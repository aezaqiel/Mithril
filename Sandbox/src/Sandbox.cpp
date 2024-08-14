#include "Sandbox.hpp"

Sandbox::Sandbox()
{
}

Sandbox::~Sandbox()
{
}

Mithril::Application* Mithril::CreateApplication()
{
    return new Sandbox();
}
