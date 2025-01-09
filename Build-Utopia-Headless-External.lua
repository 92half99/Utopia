-- UtopiaExternal-Headless.lua

IncludeDir = {}
IncludeDir["glm"] = "../vendor/glm"
IncludeDir["spdlog"] = "../vendor/spdlog/include"

group "Dependencies"
   include "vendor/yaml-cpp"
group ""

group "Core"
    include "Utopia/Build-Utopia-Headless.lua"
    -- Optional modules
    if os.isfile("Utopia-Modules/Utopia-Networking/Build-Utopia-Networking.lua") then
        include "Utopia-Modules/Utopia-Networking/Build-Utopia-Networking.lua"
    end
group ""