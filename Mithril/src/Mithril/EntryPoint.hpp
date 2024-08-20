#pragma once

#include "Core/Application.hpp"

extern Mithril::Application* Mithril::CreateApplication();

int main(void)
{
    Mithril::Application* app = Mithril::CreateApplication();
    app->Run();
    delete app;
}
