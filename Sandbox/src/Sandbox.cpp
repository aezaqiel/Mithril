#include "Sandbox.hpp"

#include <Mithril/EntryPoint.hpp>

Sandbox::Sandbox()
{
    m_Layer = Mithril::CreateRef<SandboxLayer>();
    PushLayer(m_Layer);

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
