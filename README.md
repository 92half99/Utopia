# Utopia 🌌

Utopia is a lightweight application framework built with Dear ImGui and designed to integrate seamlessly with Vulkan. This combination enables the effortless blending of real-time Vulkan rendering with a powerful UI library to create desktop applications. The goal is to expand Utopia with utilities to simplify the development of immediate-mode desktop apps and straightforward Vulkan-based applications.

Currently, Utopia supports Windows, macOS and Linux. The setup scripts provided are tailored for Visual Studio 2022.

## Requirements
- [Visual Studio 2022](https://visualstudio.com) (Setup scripts are designed for this IDE.)
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) (Preferably a recent version)

## Getting Started
1. Clone the repository with all its submodules: 

`git clone --recurse-submodules https://github.com/92half99/Utopia`

2. Run `scripts/Setup-ExampleProject.bat` to generate Visual Studio 2022 solution and project files.
3. Open the solution and run the UtopiaApp project to explore a basic example (found in UtopiaApp.cpp).

### 3rd party libaries
- [GLFW](https://github.com/glfw/glfw)
- [GLM](https://github.com/g-truc/glm)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [spdlog](https://github.com/gabime/spdlog)
- [stb](https://github.com/nothings/stb)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
  
### Additional Information
- Utopia uses the [Roboto](https://fonts.google.com/specimen/Roboto) font, licensed under the ([Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0)).
