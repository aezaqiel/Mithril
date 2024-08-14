#pragma once

#include <memory>

#include "Events/Event.hpp"
#include "Window.hpp"

namespace Mithril {

    class Application
    {
    public:
        Application();
        ~Application() = default;

        void Run();

        void OnEvent(Event& event);

        static Application& Get() { return *s_Instance; }

    private:
        bool m_Running { true };
        bool m_Suspended { false };

        std::shared_ptr<Window> m_Window;

    private:
        static Application* s_Instance;
    };

    Application* CreateApplication();

}
