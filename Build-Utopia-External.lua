-- UtopiaExternal.lua

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["glm"] = "../vendor/glm"
IncludeDir["spdlog"] = "../vendor/spdlog/include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"

group "Dependencies"
   include "vendor/imgui"
   include "vendor/glfw"
   include "vendor/yaml-cpp"
group ""

group "Core"
    include "Utopia/Build-Utopia.lua"
    
    -- Optional modules
    if os.isfile("Utopia-Modules/Utopia-Networking/Build-Utopia-Networking.lua") then
        include "Utopia-Modules/Utopia-Networking/Build-Utopia-Networking.lua"
    end
group ""