#pragma once

#include <Mithril/Mithril.hpp>

#include "SandboxLayer.hpp"

class Sandbox : public Mithril::Application
{
public:
    Sandbox();
    ~Sandbox();
private:
    Mithril::Ref<SandboxLayer> m_Layer;
};
