-- Utopia.lua
project "Utopia"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   files
   {
       "Source/**.h",
       "Source/**.hpp",
       "Source/**.cpp",

       "Platform/GUI/**.h",
       "Platform/GUI/**.hpp",
       "Platform/GUI/**.cpp",
   }

   includedirs
   {
      "Source",
      "Platform/GUI",

      "../vendor/imgui",
      "../vendor/glfw/include",
      "../vendor/stb",

      "%{IncludeDir.VulkanSDK}",
      "%{IncludeDir.glm}",
      "%{IncludeDir.spdlog}",
   }

   links
   {
       "ImGui",
       "GLFW",

       "%{Library.Vulkan}",
   }

   targetdir ("../../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../../bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "UT_PLATFORM_WINDOWS" }
      buildoptions { "/utf-8" }

   filter "configurations:Debug"
      defines { "UT_DEBUG" }
      runtime "Debug"
      symbols "On"

   filter "configurations:Release"
      defines { "UT_RELEASE" }
      runtime "Release"
      optimize "On"
      symbols "On"

   filter "configurations:Dist"
      defines { "UT_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"