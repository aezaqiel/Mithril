#include "Input.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Application.hpp"

namespace Mithril {

    bool Input::KeyPressed(KeyCode keyCode)
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::Instance().GetWindow()->Native());
        i32 state = glfwGetKey(window, static_cast<i32>(keyCode));
        return state == GLFW_PRESS;
    }

    bool Input::MouseButtonPressed(MouseCode mouseCode)
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::Instance().GetWindow()->Native());
        i32 state = glfwGetMouseButton(window, static_cast<i32>(mouseCode));
        return state == GLFW_PRESS;
    }

    glm::vec2 Input::MousePos()
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::Instance().GetWindow()->Native());
        f64 x, y;
        glfwGetCursorPos(window, &x, &y);
        return glm::vec2(static_cast<f32>(x), static_cast<f32>(y));
    }

    f32 Input::MouseX()
    {
        return MousePos().x;
    }

    f32 Input::MouseY()
    {
        return MousePos().y;
    }

}
