#include "Input.hpp"

#include "Utopia/ApplicationGUI.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Utopia {

    bool Input::IsKeyDown(KeyCode keycode)
    {
        GLFWwindow* windowHandle = Application::Get().GetWindowHandle();
        int state = glfwGetKey(windowHandle, static_cast<int>(keycode));
        return (state == GLFW_PRESS || state == GLFW_REPEAT);
    }

    bool Input::IsMouseButtonDown(MouseButton button)
    {
        GLFWwindow* windowHandle = Application::Get().GetWindowHandle();
        int state = glfwGetMouseButton(windowHandle, static_cast<int>(button));
        return (state == GLFW_PRESS);
    }

    glm::vec2 Input::GetMousePosition()
    {
        GLFWwindow* windowHandle = Application::Get().GetWindowHandle();
        double xPos = 0.0;
        double yPos = 0.0;
        glfwGetCursorPos(windowHandle, &xPos, &yPos);
        return glm::vec2(static_cast<float>(xPos), static_cast<float>(yPos));
    }

    void Input::SetCursorMode(CursorMode mode)
    {
        GLFWwindow* windowHandle = Application::Get().GetWindowHandle();
        // The + (int)mode offset is a trick to map:
        //    CursorMode::Normal -> GLFW_CURSOR_NORMAL (0)
        //    CursorMode::Hidden -> GLFW_CURSOR_HIDDEN (1)
        //    CursorMode::Locked -> GLFW_CURSOR_DISABLED (2)
        glfwSetInputMode(windowHandle, GLFW_CURSOR,
            GLFW_CURSOR_NORMAL + static_cast<int>(mode));
    }

} // namespace Utopia
