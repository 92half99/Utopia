#pragma once

#include "Layer.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "imgui.h"
#include "vulkan/vulkan.h"

// Forward declarations
struct GLFWwindow;

void check_vk_result(VkResult err);

namespace Utopia {

    struct ApplicationSpecification
    {
        std::string Name = "Utopia App";
        std::uint32_t Width = 1600;
        std::uint32_t Height = 900;
    };

    class Application
    {
    public:
        explicit Application(const ApplicationSpecification& applicationSpecification = ApplicationSpecification());
        ~Application() noexcept;

        static Application& Get();

        void Run();
        void SetMenubarCallback(const std::function<void()>& menubarCallback) { m_MenubarCallback = menubarCallback; }

        template<typename T>
        void PushLayer()
        {
            static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
            auto layer = std::make_shared<T>();
            m_LayerStack.emplace_back(layer);
            layer->OnAttach();
        }

        void PushLayer(const std::shared_ptr<Layer>& layer)
        {
            m_LayerStack.emplace_back(layer);
            layer->OnAttach();
        }

        void Close();

        [[nodiscard]] float GetTime() const noexcept;
        [[nodiscard]] GLFWwindow* GetWindowHandle() const noexcept { return m_WindowHandle; }

        [[nodiscard]] static VkInstance GetInstance() noexcept;
        [[nodiscard]] static VkPhysicalDevice GetPhysicalDevice() noexcept;
        [[nodiscard]] static VkDevice GetDevice() noexcept;

        static VkCommandBuffer GetCommandBuffer(bool begin);
        static void FlushCommandBuffer(VkCommandBuffer commandBuffer);

        static void SubmitResourceFree(std::function<void()>&& func);

    private:
        void Init();
        void Shutdown() noexcept;

    private:
        ApplicationSpecification m_Specification;
        GLFWwindow* m_WindowHandle = nullptr;
        bool m_Running = false;

        float m_TimeStep = 0.0f;
        float m_FrameTime = 0.0f;
        float m_LastFrameTime = 0.0f;

        std::vector<std::shared_ptr<Layer>> m_LayerStack;
        std::function<void()> m_MenubarCallback;
    };

    // Implemented by CLIENT
    Application* CreateApplication(int argc, char** argv);

} // namespace Utopia
