#include "Sandbox.hpp"

#include <Mithril/EntryPoint.hpp>

Sandbox::Sandbox()
{
    MINFO("Client application created");
}

Sandbox::~Sandbox()
{
    MINFO("Client application destroyed");
}

Mithril::Application* Mithril::CreateApplication()
{
    return new Sandbox();
}
