#include "Application.hpp"

#include "Logger.hpp"
#include "Events/WindowEvent.hpp"
#include "Input.hpp"
#include "Mithril/Core/KeyCodes.hpp"

namespace Mithril {

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        if (s_Instance) {
            M_CORE_ERROR("Can only have one application");
        }

        s_Instance = this;

        Logger::Init();

        m_LayerStack = std::make_unique<LayerStack>();

        m_Window = std::make_shared<Window>();
        m_Window->SetEventCallbackFn([this](auto&&... args) -> decltype(auto) { return this->OnEvent(std::forward<decltype(args)>(args)...); });

        M_CORE_INFO("Core application created");
    }

    // TODO: independent update and render loop
    // TODO: delta time
    void Application::Run()
    {
        while (m_Running) {
            m_Window->Update();

            if (!m_Suspended) {
                for (const auto& layer : *m_LayerStack) {
                    layer->OnUpdate(0.0f);
                }

                // TODO: renderer draw

                if (Input::KeyPressed(Key::Space)) {
                    M_CORE_TRACE("Polled for <SPACE>");
                }
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

        for (auto it = m_LayerStack->rbegin(); it != m_LayerStack->rend(); ++it) {
            if (event.Handled) break;
            (*it)->OnEvent(event);
        }
    }

    void Application::PushLayer(const std::shared_ptr<Layer>& layer)
    {
        m_LayerStack->PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(const std::shared_ptr<Layer>& overlay)
    {
        m_LayerStack->PushOverlay(overlay);
        overlay->OnAttach();
    }

    void Application::PopLayer(const std::shared_ptr<Layer>& layer)
    {
        m_LayerStack->PopLayer(layer);
        layer->OnDetach();
    }

    void Application::PopOverlay(const std::shared_ptr<Layer>& overlay)
    {
        m_LayerStack->PopLayer(overlay);
        overlay->OnDetach();
    }

}
