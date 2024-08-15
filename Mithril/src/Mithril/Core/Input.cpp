#include "Input.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Application.hpp"

namespace Mithril {

    bool Input::KeyPressed(KeyCode keyCode)
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::Instance().GetWindow()->Native());
        int state = glfwGetKey(window, static_cast<int>(keyCode));
        return state == GLFW_PRESS;
    }

    bool Input::MouseButtonPressed(MouseCode mouseCode)
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::Instance().GetWindow()->Native());
        int state = glfwGetMouseButton(window, static_cast<int>(mouseCode));
        return state == GLFW_PRESS;
    }

    glm::vec2 Input::MousePos()
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(Application::Instance().GetWindow()->Native());
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return glm::vec2(static_cast<float>(x), static_cast<float>(y));
    }

    float Input::MouseX()
    {
        return MousePos().x;
    }

    float Input::MouseY()
    {
        return MousePos().y;
    }

}
