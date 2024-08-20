#pragma once
#include "Mithril/Defines.hpp"

#include <string>
#include <functional>

#include "Events/Event.hpp"

struct GLFWwindow;

namespace Mithril {

    struct WindowProps
    {
        u32 Width;
        u32 Height;
        std::string Title;

        WindowProps(u32 width = 1280, u32 height = 720, const std::string& title = "Window")
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

        u32 Width() const { return m_Data.Width; }
        u32 Height() const { return m_Data.Height; }

        void* Native() const { return m_Window; }

    private:
        GLFWwindow* m_Window;

        struct WindowData
        {
            u32 Width;
            u32 Height;
            std::string Title;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
    
    private:
        static u32 s_WindowCount;
    };

}
