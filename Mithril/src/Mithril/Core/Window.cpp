#include "Window.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Logger.hpp"
#include "Events/WindowEvent.hpp"
#include "Events/KeyEvent.hpp"
#include "Events/MouseEvent.hpp"

namespace Mithril {

    uint32_t Window::s_WindowCount = 0;

    Window::Window(WindowProps props)
    {
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;
        m_Data.Title = props.Title;

        glfwSetErrorCallback([](int code, const char* desc) -> void
        {
            M_CORE_ERROR("GLFW Error ({}) {}", code, desc);
        });

        if (s_WindowCount == 0) {
            glfwInit();
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
        s_WindowCount++;

        glfwSetWindowUserPointer(m_Window, &m_Data);

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) -> void
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            WindowResizeEvent e(width, height);
            data.EventCallback(e);
        });

        glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int x, int y) -> void
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            WindowMoveEvent e(x, y);
            data.EventCallback(e);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) -> void
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            WindowCloseEvent e;
            data.EventCallback(e);
        });

        glfwSetWindowIconifyCallback(m_Window, [](GLFWwindow* window, int iconified) -> void
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            if (iconified == GLFW_TRUE) {
                WindowMinimizeEvent e;
                data.EventCallback(e);
            } else if (iconified == GLFW_FALSE) {
                WindowRestoreEvent e;
                data.EventCallback(e);
            } else {
                M_CORE_ERROR("Unknown glfw iconify callback");
            }
        });

        glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused) -> void
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            if (focused == GLFW_TRUE) {
                WindowFocusEvent e;
                data.EventCallback(e);
            } else if (focused == GLFW_FALSE) {
                WindowLostFocusEvent e;
                data.EventCallback(e);
            } else {
                M_CORE_ERROR("Unknown glfw focus callback");
            }
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) -> void
        {
            (void)scancode;
            (void)mods;

            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action) {
                case GLFW_PRESS: {
                    KeyPressedEvent e(key, false);
                    data.EventCallback(e);
                } break;
                case GLFW_RELEASE: {
                    KeyReleasedEvent e(key);
                    data.EventCallback(e);
                } break;
                case GLFW_REPEAT: {
                    KeyPressedEvent e(key, true);
                    data.EventCallback(e);
                } break;
                default:
                    M_CORE_ERROR("Unknown action from glfw key callback");
            }
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int key) -> void
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            KeyTypedEvent e(key);
            data.EventCallback(e);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) -> void
        {
            (void)mods;

            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action) {
                case GLFW_PRESS: {
                    MouseButtonPressedEvent e(button);
                    data.EventCallback(e);
                } break;
                case GLFW_RELEASE: {
                    MouseButtonReleasedEvent e(button);
                    data.EventCallback(e);
                } break;
                default:
                    M_CORE_ERROR("Unknown action from glfw mouse button callback");
            }
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y) -> void
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseMovedEvent e(x, y);
            data.EventCallback(e);
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double x, double y) -> void
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseScrolledEvent e(x, y);
            data.EventCallback(e);
        });
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_Window);
        s_WindowCount--;

        if (s_WindowCount == 0) {
            glfwTerminate();
        }
    }

    void Window::Update()
    {
        glfwPollEvents();
    }

}
