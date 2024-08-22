#include "Application.hpp"

#include "Logger.hpp"
#include "Events/WindowEvent.hpp"
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

        m_Window = CreateRef<Window>();
        m_Window->SetEventCallbackFn([this](auto&&... args) -> decltype(auto) { return this->OnEvent(std::forward<decltype(args)>(args)...); });

        m_Renderer = CreateRef<Renderer>();

        M_CORE_INFO("Core application created");
    }

    // TODO: independent update and render loop
    void Application::Run()
    {
        while (m_Running) {
            std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();
            f32 dt = static_cast<f32>(std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - m_LastTime).count()) / 1000000000.0f;
            m_LastTime = currentTime;

            m_Window->Update();

            if (!m_Suspended) {
                for (const auto& layer : m_LayerStack) {
                    layer->OnUpdate(dt);
                }

                m_Renderer->Draw();
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

            m_Renderer->Resize(e.Width(), e.Height());

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

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
            if (event.Handled) break;
            (*it)->OnEvent(event);
        }
    }

    void Application::PushLayer(const std::shared_ptr<Layer>& layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(const std::shared_ptr<Layer>& overlay)
    {
        m_LayerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }

    void Application::PopLayer(const std::shared_ptr<Layer>& layer)
    {
        m_LayerStack.PopLayer(layer);
        layer->OnDetach();
    }

    void Application::PopOverlay(const std::shared_ptr<Layer>& overlay)
    {
        m_LayerStack.PopLayer(overlay);
        overlay->OnDetach();
    }

}
