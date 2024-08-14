#pragma once

#include <string>
#include <functional>

#include "Events/Event.hpp"

struct GLFWwindow;

namespace Mithril {

    struct WindowProps
    {
        uint32_t Width;
        uint32_t Height;
        std::string Title;

        WindowProps(uint32_t width = 1280, uint32_t height = 720, const std::string& title = "Window")
            : Width(width), Height(height), Title(title) {}
    };

    class Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        Window(WindowProps props = WindowProps());
        ~Window();

        void Update();

        void SetEventCallbackFn(EventCallbackFn callback) { m_Data.EventCallback = callback; }

        uint32_t Width() const { return m_Data.Width; }
        uint32_t Height() const { return m_Data.Height; }

        void* Native() const { return m_Window; }

    private:
        GLFWwindow* m_Window;

        struct WindowData
        {
            uint32_t Width;
            uint32_t Height;
            std::string Title;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
    
    private:
        static uint32_t s_WindowCount;
    };

}
