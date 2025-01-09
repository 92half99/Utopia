-- Utopia-ExampleProject.lua
workspace "UtopiaApp"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "UtopiaApp"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Build-Utopia-External.lua"
include "UtopiaApp/Build-Utopia-App.lua"