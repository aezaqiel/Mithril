#pragma once
#include "Mithril/Defines.hpp"

#include <glm/glm.hpp>

#include "KeyCodes.hpp"
#include "MouseCodes.hpp"

namespace Mithril {

    class Input
    {
    public:
        static bool KeyPressed(KeyCode keyCode);
        static bool MouseButtonPressed(MouseCode mouseCode);
        static glm::vec2 MousePos();
        static f32 MouseX();
        static f32 MouseY();
    private:
    };

}
