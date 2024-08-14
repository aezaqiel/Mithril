#pragma once

#include "Events/Event.hpp"

namespace Mithril {

    class Application
    {
    public:
        Application();
        ~Application() = default;

        void Run();

        void OnEvent(Event& event);

    private:
        bool m_Running { true };
        bool m_Suspended { false };

        static Application* s_Instance;
    };

    Application* CreateApplication();

}
