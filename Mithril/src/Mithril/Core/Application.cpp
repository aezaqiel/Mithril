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

    void Application::Run()
    {
        M_CORE_DEBUG("Running...");
    }

}
