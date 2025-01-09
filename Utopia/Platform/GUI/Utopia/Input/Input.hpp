#pragma once

#include "KeyCodes.hpp"
#include <glm/glm.hpp>

namespace Utopia {

    class Input
    {
    public:
        // Returns true if the specified key is down (pressed or held).
        [[nodiscard]] static bool IsKeyDown(KeyCode keycode);

        // Returns true if the specified mouse button is currently down.
        [[nodiscard]] static bool IsMouseButtonDown(MouseButton button);

        // Returns the current mouse position as a vec2.
        [[nodiscard]] static glm::vec2 GetMousePosition();

        // Sets the cursor mode (Normal, Hidden, Locked).
        static void SetCursorMode(CursorMode mode);
    };

} // namespace Utopia
