#include "Application.hpp"

// Adapted from Dear ImGui Vulkan example

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <memory>
#include <functional>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <iostream>

// Embedded font
#include "ImGui/Roboto-Regular.embed"

extern bool g_ApplicationRunning;

//#define IMGUI_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
#endif

static VkAllocationCallbacks* g_Allocator = nullptr;
static VkInstance                  g_Instance = VK_NULL_HANDLE;
static VkPhysicalDevice            g_PhysicalDevice = VK_NULL_HANDLE;
static VkDevice                    g_Device = VK_NULL_HANDLE;
static std::uint32_t               g_QueueFamily = static_cast<std::uint32_t>(-1);
static VkQueue                     g_Queue = VK_NULL_HANDLE;
static VkDebugReportCallbackEXT    g_DebugReport = VK_NULL_HANDLE;
static VkPipelineCache             g_PipelineCache = VK_NULL_HANDLE;
static VkDescriptorPool            g_DescriptorPool = VK_NULL_HANDLE;

static ImGui_ImplVulkanH_Window    g_MainWindowData;
static int                         g_MinImageCount = 2;
static bool                        g_SwapChainRebuild = false;

// Per-frame-in-flight
static std::vector<std::vector<VkCommandBuffer>> s_AllocatedCommandBuffers;
static std::vector<std::vector<std::function<void()>>> s_ResourceFreeQueue;

// Current frame index that increments every frame (not the swapchain image index)
static std::uint32_t s_CurrentFrameIndex = 0;

static Utopia::Application* s_Instance = nullptr;

// -----------------------------------------------------------------------------
// If your older backend has no debug callbacks or needs a different approach,
// simply remove or adapt #ifdef code below.
// -----------------------------------------------------------------------------
void check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    std::fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        std::abort();
}

#ifdef IMGUI_VULKAN_DEBUG_REPORT
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t object,
    size_t location,
    int32_t messageCode,
    const char* pLayerPrefix,
    const char* pMessage,
    void* pUserData)
{
    (void)flags;
    (void)object;
    (void)location;
    (void)messageCode;
    (void)pUserData;
    (void)pLayerPrefix; // Unused arguments
    std::fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
    return VK_FALSE;
}
#endif // IMGUI_VULKAN_DEBUG_REPORT

//
// 1) Minimal SetupVulkan() + SetupVulkanWindow() that matches your older backend
//    (calls a single-argument ImGui_ImplVulkan_Init() later).
//
static void SetupVulkan(const char** extensions, std::uint32_t extensions_count)
{
    VkResult err;

    // Create Vulkan Instance
    {
        VkInstanceCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.enabledExtensionCount = extensions_count;
        create_info.ppEnabledExtensionNames = extensions;

#ifdef IMGUI_VULKAN_DEBUG_REPORT
        // Enabling validation layers
        const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
        create_info.enabledLayerCount = 1;
        create_info.ppEnabledLayerNames = layers;

        // Enable debug report extension
        std::vector<const char*> extended(extensions, extensions + extensions_count);
        extended.push_back("VK_EXT_debug_report");
        create_info.enabledExtensionCount = static_cast<uint32_t>(extended.size());
        create_info.ppEnabledExtensionNames = extended.data();

        err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
        check_vk_result(err);

        auto vkCreateDebugReportCallbackEXT =
            reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
                vkGetInstanceProcAddr(g_Instance, "vkCreateDebugReportCallbackEXT"));
        IM_ASSERT(vkCreateDebugReportCallbackEXT != nullptr);

        VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
        debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT
            | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        debug_report_ci.pfnCallback = debug_report;
        debug_report_ci.pUserData = nullptr;
        err = vkCreateDebugReportCallbackEXT(
            g_Instance, &debug_report_ci, g_Allocator, &g_DebugReport);
        check_vk_result(err);
#else
        // No debug
        err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
        check_vk_result(err);
        IM_UNUSED(g_DebugReport);
#endif
    }

    // Select a GPU
    {
        uint32_t gpu_count = 0;
        err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, nullptr);
        check_vk_result(err);
        IM_ASSERT(gpu_count > 0);

        std::vector<VkPhysicalDevice> gpus(gpu_count);
        err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus.data());
        check_vk_result(err);

        // If multiple GPUs, pick a discrete if available
        int use_gpu = 0;
        for (int i = 0; i < static_cast<int>(gpu_count); i++)
        {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(gpus[i], &props);
            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                use_gpu = i;
                break;
            }
        }
        g_PhysicalDevice = gpus[use_gpu];
    }

    // Select graphics queue family
    {
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, nullptr);
        std::vector<VkQueueFamilyProperties> queue_props(count);
        vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, queue_props.data());

        for (uint32_t i = 0; i < count; i++)
        {
            if (queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                g_QueueFamily = i;
                break;
            }
        }
        IM_ASSERT(g_QueueFamily != static_cast<std::uint32_t>(-1));
    }

    // Create Logical Device
    {
        const float queue_priority[] = { 1.0f };
        VkDeviceQueueCreateInfo queue_info = {};
        queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.queueFamilyIndex = g_QueueFamily;
        queue_info.queueCount = 1;
        queue_info.pQueuePriorities = queue_priority;

        const char* device_ext[] = { "VK_KHR_swapchain" };
        VkDeviceCreateInfo device_ci = {};
        device_ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_ci.queueCreateInfoCount = 1;
        device_ci.pQueueCreateInfos = &queue_info;
        device_ci.enabledExtensionCount = 1;
        device_ci.ppEnabledExtensionNames = device_ext;

        err = vkCreateDevice(g_PhysicalDevice, &device_ci, g_Allocator, &g_Device);
        check_vk_result(err);

        vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
    }

    // Create Descriptor Pool
    {
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER,                1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000 }
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * (uint32_t)(IM_ARRAYSIZE(pool_sizes));
        pool_info.poolSizeCount = (uint32_t)(IM_ARRAYSIZE(pool_sizes));
        pool_info.pPoolSizes = pool_sizes;

        err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator, &g_DescriptorPool);
        check_vk_result(err);
    }
}

static void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface,
    int width, int height)
{
    wd->Surface = surface;

    // Check WSI support
    VkBool32 supported = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, wd->Surface, &supported);
    if (!supported)
    {
        std::fprintf(stderr, "Error: no WSI support\n");
        std::exit(-1);
    }

    // Choose a format
    const VkFormat requestSurfaceImageFormat[] =
    {
        VK_FORMAT_B8G8R8A8_UNORM,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_FORMAT_B8G8R8_UNORM,
        VK_FORMAT_R8G8B8_UNORM
    };
    const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

    wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
        g_PhysicalDevice, wd->Surface,
        requestSurfaceImageFormat,
        (int)IM_ARRAYSIZE(requestSurfaceImageFormat),
        requestSurfaceColorSpace);

    // Present mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
    wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
        g_PhysicalDevice, wd->Surface,
        present_modes,
        (int)(IM_ARRAYSIZE(present_modes)));

    IM_ASSERT(g_MinImageCount >= 2);

    // Create the swapchain, framebuffers, etc.
    ImGui_ImplVulkanH_CreateOrResizeWindow(
        g_Instance, g_PhysicalDevice, g_Device,
        wd,
        g_QueueFamily, g_Allocator,
        width, height,
        g_MinImageCount);
}

static void CleanupVulkan()
{
    vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);

#ifdef IMGUI_VULKAN_DEBUG_REPORT
    auto destroyCb = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
        vkGetInstanceProcAddr(g_Instance, "vkDestroyDebugReportCallbackEXT"));
    if (destroyCb)
        destroyCb(g_Instance, g_DebugReport, g_Allocator);
#endif
    vkDestroyDevice(g_Device, g_Allocator);
    vkDestroyInstance(g_Instance, g_Allocator);
}

static void CleanupVulkanWindow()
{
    ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, &g_MainWindowData, g_Allocator);
}

static void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data)
{
    VkResult err;
    VkSemaphore acquire_sem = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
    VkSemaphore release_sem = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;

    // Acquire next image
    err = vkAcquireNextImageKHR(g_Device, wd->Swapchain, UINT64_MAX, acquire_sem,
        VK_NULL_HANDLE, &wd->FrameIndex);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
    {
        g_SwapChainRebuild = true;
        return;
    }
    check_vk_result(err);

    s_CurrentFrameIndex = (s_CurrentFrameIndex + 1) % g_MainWindowData.ImageCount;
    ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];

    // Wait & reset fence
    {
        err = vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);
        check_vk_result(err);
        err = vkResetFences(g_Device, 1, &fd->Fence);
        check_vk_result(err);
    }

    // Free resources queued for deletion
    {
        for (auto& func : s_ResourceFreeQueue[s_CurrentFrameIndex])
            func();
        s_ResourceFreeQueue[s_CurrentFrameIndex].clear();
    }
    // Free any command buffers from GetCommandBuffer
    {
        auto& allocated = s_AllocatedCommandBuffers[wd->FrameIndex];
        if (!allocated.empty())
        {
            vkFreeCommandBuffers(g_Device, fd->CommandPool,
                (uint32_t)allocated.size(),
                allocated.data());
            allocated.clear();
        }
        err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
        check_vk_result(err);
    }
    // Begin command buffer
    {
        VkCommandBufferBeginInfo binfo = {};
        binfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        binfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(fd->CommandBuffer, &binfo);
        check_vk_result(err);
    }
    {
        VkRenderPassBeginInfo rpinfo = {};
        rpinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rpinfo.renderPass = wd->RenderPass;
        rpinfo.framebuffer = fd->Framebuffer;
        rpinfo.renderArea.extent.width = wd->Width;
        rpinfo.renderArea.extent.height = wd->Height;
        rpinfo.clearValueCount = 1;
        rpinfo.pClearValues = &wd->ClearValue;
        vkCmdBeginRenderPass(fd->CommandBuffer, &rpinfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    // Record ImGui draws
    ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

    // End and submit
    vkCmdEndRenderPass(fd->CommandBuffer);
    {
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &acquire_sem;
        submit_info.pWaitDstStageMask = &wait_stage;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &fd->CommandBuffer;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &release_sem;

        err = vkEndCommandBuffer(fd->CommandBuffer);
        check_vk_result(err);
        err = vkQueueSubmit(g_Queue, 1, &submit_info, fd->Fence);
        check_vk_result(err);
    }
}

static void FramePresent(ImGui_ImplVulkanH_Window* wd)
{
    if (g_SwapChainRebuild)
        return;

    VkSemaphore release_sem = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
    VkPresentInfoKHR pres_info = {};
    pres_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    pres_info.waitSemaphoreCount = 1;
    pres_info.pWaitSemaphores = &release_sem;
    pres_info.swapchainCount = 1;
    pres_info.pSwapchains = &wd->Swapchain;
    pres_info.pImageIndices = &wd->FrameIndex;

    VkResult err = vkQueuePresentKHR(g_Queue, &pres_info);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
    {
        g_SwapChainRebuild = true;
        return;
    }
    check_vk_result(err);

    wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount;
}

// GLFW Error callback
static void glfw_error_callback(int error, const char* description)
{
    std::fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

//
// 2) The code inside your Application class, but changed to match older backend signatures.
//
//    - Single-argument ImGui_ImplVulkan_Init(...)
//    - NO command buffer param in ImGui_ImplVulkan_CreateFontsTexture()
//    - Use ImGui_ImplVulkan_DestroyFontsTexture() instead of DestroyFontUploadObjects()
//
namespace Utopia {

    Application::Application(const ApplicationSpecification& specification)
        : m_Specification(specification)
    {
        s_Instance = this;
        Init();
    }

    Application::~Application() noexcept
    {
        Shutdown();
        s_Instance = nullptr;
    }

    Application& Application::Get()
    {
        return *s_Instance;
    }

    void Application::Init()
    {
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
        {
            std::cerr << "Could not initialize GLFW!\n";
            return;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_WindowHandle = glfwCreateWindow(
            (int)m_Specification.Width,
            (int)m_Specification.Height,
            m_Specification.Name.c_str(),
            nullptr,
            nullptr
        );

        // Setup Vulkan
        if (!glfwVulkanSupported())
        {
            std::cerr << "GLFW: Vulkan not supported!\n";
            return;
        }

        // Gather instance extensions
        uint32_t extensions_count = 0;
        const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
        SetupVulkan(extensions, extensions_count);

        // Create Window Surface
        VkSurfaceKHR surface;
        VkResult err = glfwCreateWindowSurface(g_Instance, m_WindowHandle, g_Allocator, &surface);
        check_vk_result(err);

        // Create swapchain + framebuffers
        int w, h;
        glfwGetFramebufferSize(m_WindowHandle, &w, &h);
        auto* wd = &g_MainWindowData;
        SetupVulkanWindow(wd, surface, w, h);

        // Prepare arrays for “N in-flight frames”
        s_AllocatedCommandBuffers.resize(wd->ImageCount);
        s_ResourceFreeQueue.resize(wd->ImageCount);

        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Init platform back-end
        ImGui_ImplGlfw_InitForVulkan(m_WindowHandle, true);

        // Prepare Vulkan init info
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = g_Instance;
        init_info.PhysicalDevice = g_PhysicalDevice;
        init_info.Device = g_Device;
        init_info.QueueFamily = g_QueueFamily;
        init_info.Queue = g_Queue;
        init_info.PipelineCache = g_PipelineCache;
        init_info.DescriptorPool = g_DescriptorPool;
        init_info.RenderPass = VK_NULL_HANDLE;    // We'll set it below if needed
        init_info.Subpass = 0;
        init_info.MinImageCount = g_MinImageCount;
        init_info.ImageCount = wd->ImageCount;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = g_Allocator;
        init_info.CheckVkResultFn = check_vk_result;

        // Because your older backend only has "bool ImGui_ImplVulkan_Init(ImGui_ImplVulkan_InitInfo* info)",
        // we do NOT pass a second parameter. Instead, store the RenderPass in the init_info if your older
        // code uses it internally:
        init_info.RenderPass = wd->RenderPass;  // store it in the init info
        ImGui_ImplVulkan_Init(&init_info);      // single-argument init

        // Load default font
        ImFontConfig font_cfg;
        font_cfg.FontDataOwnedByAtlas = false;
        ImFont* robotoFont = io.Fonts->AddFontFromMemoryTTF(
            (void*)g_RobotoRegular,
            (int)sizeof(g_RobotoRegular),
            20.0f,
            &font_cfg
        );
        io.FontDefault = robotoFont;

        // Upload Fonts
        {
            // Acquire command pool/buffer
            VkCommandPool    cmd_pool = wd->Frames[wd->FrameIndex].CommandPool;
            VkCommandBuffer  cmd_buffer = wd->Frames[wd->FrameIndex].CommandBuffer;

            err = vkResetCommandPool(g_Device, cmd_pool, 0);
            check_vk_result(err);

            VkCommandBufferBeginInfo begin_info = {};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            err = vkBeginCommandBuffer(cmd_buffer, &begin_info);
            check_vk_result(err);

            // **Older** backend: param-less CreateFontsTexture()
            ImGui_ImplVulkan_CreateFontsTexture();

            err = vkEndCommandBuffer(cmd_buffer);
            check_vk_result(err);

            VkSubmitInfo end_info = {};
            end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            end_info.commandBufferCount = 1;
            end_info.pCommandBuffers = &cmd_buffer;
            err = vkQueueSubmit(g_Queue, 1, &end_info, VK_NULL_HANDLE);
            check_vk_result(err);

            err = vkDeviceWaitIdle(g_Device);
            check_vk_result(err);

            // **Older** backend: ImGui_ImplVulkan_DestroyFontsTexture()
            ImGui_ImplVulkan_DestroyFontsTexture();
        }
    }

    void Application::Shutdown() noexcept
    {
        // Detach layers
        for (auto& layer : m_LayerStack)
            layer->OnDetach();
        m_LayerStack.clear();

        // Wait GPU
        VkResult err = vkDeviceWaitIdle(g_Device);
        check_vk_result(err);

        // Free queued resources
        for (auto& queue : s_ResourceFreeQueue)
        {
            for (auto& func : queue)
                func();
        }
        s_ResourceFreeQueue.clear();

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        CleanupVulkanWindow();
        CleanupVulkan();

        glfwDestroyWindow(m_WindowHandle);
        glfwTerminate();

        g_ApplicationRunning = false;
    }

    void Application::Run()
    {
        m_Running = true;

        auto* wd = &g_MainWindowData;
        ImVec4 clear_color(0.45f, 0.55f, 0.60f, 1.00f);
        ImGuiIO& io = ImGui::GetIO();

        while (!glfwWindowShouldClose(m_WindowHandle) && m_Running)
        {
            glfwPollEvents();

            // Let layers update
            for (auto& layer : m_LayerStack)
                layer->OnUpdate(m_TimeStep);

            // Resize?
            if (g_SwapChainRebuild)
            {
                int width = 0, height = 0;
                glfwGetFramebufferSize(m_WindowHandle, &width, &height);
                if (width > 0 && height > 0)
                {
                    ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
                    ImGui_ImplVulkanH_CreateOrResizeWindow(
                        g_Instance, g_PhysicalDevice, g_Device,
                        wd, g_QueueFamily, g_Allocator,
                        width, height, g_MinImageCount);

                    g_MainWindowData.FrameIndex = 0;
                    s_AllocatedCommandBuffers.clear();
                    s_AllocatedCommandBuffers.resize(g_MainWindowData.ImageCount);

                    g_SwapChainRebuild = false;
                }
            }

            // ImGui frame
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            {
                static ImGuiDockNodeFlags dock_flags = ImGuiDockNodeFlags_None;
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
                if (m_MenubarCallback)
                    window_flags |= ImGuiWindowFlags_MenuBar;

                const ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

                window_flags |= ImGuiWindowFlags_NoTitleBar
                    | ImGuiWindowFlags_NoCollapse
                    | ImGuiWindowFlags_NoResize
                    | ImGuiWindowFlags_NoMove
                    | ImGuiWindowFlags_NoBringToFrontOnFocus
                    | ImGuiWindowFlags_NoNavFocus;

                if (dock_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                    window_flags |= ImGuiWindowFlags_NoBackground;

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                ImGui::Begin("DockSpace Demo", nullptr, window_flags);
                ImGui::PopStyleVar();
                ImGui::PopStyleVar(2);

                if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
                {
                    ImGuiID dockspace_id = ImGui::GetID("VulkanDockspace");
                    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dock_flags);
                }

                if (m_MenubarCallback)
                {
                    if (ImGui::BeginMenuBar())
                    {
                        m_MenubarCallback();
                        ImGui::EndMenuBar();
                    }
                }

                // Render each layer's UI
                for (auto& layer : m_LayerStack)
                    layer->OnUIRender();

                ImGui::End();
            }

            // Render it
            ImGui::Render();
            ImDrawData* main_draw_data = ImGui::GetDrawData();
            const bool minimized = (main_draw_data->DisplaySize.x <= 0.0f ||
                main_draw_data->DisplaySize.y <= 0.0f);

            wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
            wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
            wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
            wd->ClearValue.color.float32[3] = clear_color.w;

            if (!minimized)
                FrameRender(wd, main_draw_data);

            // Multiple viewports
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }

            // Present
            if (!minimized)
                FramePresent(wd);

            // Timestep
            float time_now = GetTime();
            m_FrameTime = time_now - m_LastFrameTime;
            m_TimeStep = glm::min<float>(m_FrameTime, 0.0333f);
            m_LastFrameTime = time_now;
        }
    }

    void Application::Close()
    {
        m_Running = false;
    }

    float Application::GetTime() const noexcept
    {
        return (float)glfwGetTime();
    }

    VkInstance Application::GetInstance() noexcept
    {
        return g_Instance;
    }
    VkPhysicalDevice Application::GetPhysicalDevice() noexcept
    {
        return g_PhysicalDevice;
    }
    VkDevice Application::GetDevice() noexcept
    {
        return g_Device;
    }

    VkCommandBuffer Application::GetCommandBuffer(bool /*begin*/)
    {
        auto* wd = &g_MainWindowData;
        VkCommandPool cmd_pool = wd->Frames[wd->FrameIndex].CommandPool;

        VkCommandBufferAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = cmd_pool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = 1;

        auto& command_buffer = s_AllocatedCommandBuffers[wd->FrameIndex].emplace_back();
        VkResult err = vkAllocateCommandBuffers(g_Device, &alloc_info, &command_buffer);
        check_vk_result(err);

        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(command_buffer, &begin_info);
        check_vk_result(err);

        return command_buffer;
    }

    void Application::FlushCommandBuffer(VkCommandBuffer cmd)
    {
        constexpr uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000ULL;
        VkResult err = vkEndCommandBuffer(cmd);
        check_vk_result(err);

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &cmd;

        VkFenceCreateInfo fence_ci = {};
        fence_ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_ci.flags = 0;

        VkFence fence = VK_NULL_HANDLE;
        err = vkCreateFence(g_Device, &fence_ci, nullptr, &fence);
        check_vk_result(err);

        err = vkQueueSubmit(g_Queue, 1, &submit_info, fence);
        check_vk_result(err);

        err = vkWaitForFences(g_Device, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);
        check_vk_result(err);

        vkDestroyFence(g_Device, fence, nullptr);
    }

    void Application::SubmitResourceFree(std::function<void()>&& func)
    {
        s_ResourceFreeQueue[s_CurrentFrameIndex].push_back(std::move(func));
    }

} // namespace Utopia
