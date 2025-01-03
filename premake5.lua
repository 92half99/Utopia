-- premake5.lua
workspace "UtopiaApp"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "UtopiaApp"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "UtopiaExternal.lua"
include "UtopiaApp"