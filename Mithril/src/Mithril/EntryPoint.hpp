#pragma once

#include "Core/Application.hpp"

extern Mithril::Application* Mithril::CreateApplication();

int main()
{
    Mithril::Application* app = Mithril::CreateApplication();
    app->Run();
    delete app;
}
