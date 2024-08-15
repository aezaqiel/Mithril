#pragma once

#include <memory>

#include "Events/Event.hpp"
#include "Window.hpp"
#include "LayerStack.hpp"
#include "Mithril/Renderer/Renderer.hpp"

namespace Mithril {

    class Application
    {
    public:
        Application();
        ~Application() = default;

        void Run();

        void OnEvent(Event& event);

        void PushLayer(const std::shared_ptr<Layer>& layer);
        void PushOverlay(const std::shared_ptr<Layer>& overlay);
        void PopLayer(const std::shared_ptr<Layer>& layer);
        void PopOverlay(const std::shared_ptr<Layer>& overlay);

        const std::shared_ptr<Window>& GetWindow() const { return m_Window; }

        static Application& Instance() { return *s_Instance; }

    private:
        bool m_Running { true };
        bool m_Suspended { false };

        LayerStack m_LayerStack;

        std::shared_ptr<Window> m_Window;
        std::shared_ptr<Renderer> m_Renderer;

    private:
        static Application* s_Instance;
    };

    Application* CreateApplication();

}
