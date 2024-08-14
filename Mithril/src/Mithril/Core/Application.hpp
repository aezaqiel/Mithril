#pragma once

namespace Mithril {

    class Application
    {
    public:
        Application();
        ~Application() = default;

        void Run();

    private:
        static Application* s_Instance;
    };

    Application* CreateApplication();

}
