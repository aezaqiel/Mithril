#include "Application.hpp"

#include "Logger.hpp"

namespace Mithril {

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        if (s_Instance) {
            M_CORE_ERROR("Can only have one application");
        }

        s_Instance = this;

        Logger::Init();
        M_CORE_INFO("Core application created");
    }

    // TODO: independent update and render loop
    void Application::Run()
    {
        while (m_Running) {
            // TODO: Tick

            if (!m_Suspended) {
                // TODO: Update
                // TODO: Render
            }
        }
    }

    void Application::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
    }

}
