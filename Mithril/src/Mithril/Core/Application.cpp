#include "Application.hpp"

#include "Logger.hpp"

#include "Events/WindowEvent.hpp"

namespace Mithril {

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        if (s_Instance) {
            M_CORE_ERROR("Can only have one application");
        }

        s_Instance = this;

        Logger::Init();

        m_Window = std::make_shared<Window>();
        m_Window->SetEventCallbackFn([this](auto&&... args) -> decltype(auto) { return this->OnEvent(std::forward<decltype(args)>(args)...); });

        M_CORE_INFO("Core application created");
    }

    // TODO: independent update and render loop
    void Application::Run()
    {
        while (m_Running) {
            m_Window->Update();

            if (!m_Suspended) {
                // TODO: layer stack update
                // TODO: renderer draw
            }
        }
    }

    void Application::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<WindowCloseEvent>([&](WindowCloseEvent&) -> bool
        {
            m_Running = false;
            return true;
        });

        dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent& e) -> bool
        {
            if (e.Width() == 0 || e.Height() == 0) {
                m_Suspended = true;
                return true;
            }

            m_Suspended = false;

            // TODO: renderer resize

            return false;
        });

        dispatcher.Dispatch<WindowMinimizeEvent>([&](WindowMinimizeEvent&) -> bool
        {
            m_Suspended = true;
            return true;
        });

        dispatcher.Dispatch<WindowRestoreEvent>([&](WindowRestoreEvent&) -> bool
        {
            m_Suspended = false;
            return true;
        });
    }

}
