# Utopia

Utopia is a lightweight application framework built with Dear ImGui and designed to integrate seamlessly with Vulkan. This combination enables the effortless blending of real-time Vulkan rendering with a powerful UI library to create desktop applications. The goal is to expand Utopia with utilities to simplify the development of immediate-mode desktop apps and straightforward Vulkan-based applications.

Currently, Utopia supports Windows, with plans to add macOS and Linux support in the future. The setup scripts provided are tailored for Visual Studio 2022.

## Requirements
- [Visual Studio 2022](https://visualstudio.com) (Optional: Setup scripts are designed for this IDE.)
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) (Preferably a recent version)

## Getting Started
1. Clone the repository with all its submodules: 

`git clone --recurse-submodules https://github.com/92half99/Utopia`

2. Run `scripts/Setup.bat` to generate Visual Studio 2022 solution and project files.
3. Open the solution and run the UtopiaApp project to explore a basic example (found in UtopiaApp.cpp).

### 3rd party libaries
- [Dear ImGui](https://github.com/ocornut/imgui)
- [GLFW](https://github.com/glfw/glfw)
- [stb](https://github.com/nothings/stb)
- [GLM](https://github.com/g-truc/glm) (included for convenience)

### Additional Information
- Utopia uses the [Roboto](https://fonts.google.com/specimen/Roboto) font, licensed under the ([Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0)).
