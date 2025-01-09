-- Utopia-Headless.lua
project "Utopia-Headless"
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

       "Platform/Headless/**.h",
       "Platform/Headless/**.hpp",
       "Platform/Headless/**.cpp",
   }

   includedirs
   {
      "Source",
      "Platform/Headless",

      "%{IncludeDir.glm}",
      "%{IncludeDir.spdlog}",
   }

   links
   {
   }

   defines { "UT_HEADLESS" }

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "UT_PLATFORM_WINDOWS" }
      buildoptions { "/utf-8" }

   filter "system:linux"
      systemversion "latest"
      defines { "UT_PLATFORM_LINUX" }

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