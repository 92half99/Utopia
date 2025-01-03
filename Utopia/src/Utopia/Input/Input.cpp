#include "Input.hpp"
#include "Utopia/Application.hpp"

#include <GLFW/glfw3.h>

namespace Utopia {

    bool Input::IsKeyDown(KeyCode keycode) noexcept
    {
        GLFWwindow* windowHandle = Application::Get().GetWindowHandle();
        int state = glfwGetKey(windowHandle, static_cast<int>(keycode));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::IsMouseButtonDown(MouseButton button) noexcept
    {
        GLFWwindow* windowHandle = Application::Get().GetWindowHandle();
        int state = glfwGetMouseButton(windowHandle, static_cast<int>(button));
        return state == GLFW_PRESS;
    }

    glm::vec2 Input::GetMousePosition() noexcept
    {
        GLFWwindow* windowHandle = Application::Get().GetWindowHandle();

        double x, y;
        glfwGetCursorPos(windowHandle, &x, &y);
        return { static_cast<float>(x), static_cast<float>(y) };
    }

    void Input::SetCursorMode(CursorMode mode) noexcept
    {
        GLFWwindow* windowHandle = Application::Get().GetWindowHandle();
        glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL + static_cast<int>(mode));
    }

} // namespace Utopia
