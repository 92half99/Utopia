#pragma once

#include "KeyCodes.hpp"
#include <glm/glm.hpp>

namespace Utopia {

    class Input
    {
    public:
        static bool IsKeyDown(KeyCode keycode) noexcept;
        static bool IsMouseButtonDown(MouseButton button) noexcept;

        static glm::vec2 GetMousePosition() noexcept;

        static void SetCursorMode(CursorMode mode) noexcept;
    };

} // namespace Utopia
