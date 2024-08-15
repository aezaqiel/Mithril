#include "SandboxLayer.hpp"

void SandboxLayer::OnAttach()
{
    MINFO("{} layer attached", Name());
}

void SandboxLayer::OnDetach()
{
    MINFO("{} layer detached", Name());
}

void SandboxLayer::OnUpdate(float)
{
}

void SandboxLayer::OnEvent(Mithril::Event&)
{
}
