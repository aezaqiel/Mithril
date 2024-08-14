#include "Application.hpp"

#include <iostream>

namespace Mithril {

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        if (s_Instance) {
            std::cerr << "Can only have one application" << std::endl;
        }

        s_Instance = this;
    }

    void Application::Run()
    {
        std::cout << "Running..." << std::endl;
    }

}
