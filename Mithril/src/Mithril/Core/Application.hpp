#pragma once
#include "Mithril/Defines.hpp"

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

        void PushLayer(const Ref<Layer>& layer);
        void PushOverlay(const Ref<Layer>& overlay);
        void PopLayer(const Ref<Layer>& layer);
        void PopOverlay(const Ref<Layer>& overlay);

        const Ref<Window>& GetWindow() const { return m_Window; }

        static Application& Instance() { return *s_Instance; }

    private:
        bool m_Running { true };
        bool m_Suspended { false };

        LayerStack m_LayerStack;

        Ref<Window> m_Window;
        Ref<Renderer> m_Renderer;

    private:
        static Application* s_Instance;
    };

    Application* CreateApplication();

}
