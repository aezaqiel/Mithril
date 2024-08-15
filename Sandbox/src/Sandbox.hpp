#pragma once

#include <Mithril/Mithril.hpp>

#include "SandboxLayer.hpp"

class Sandbox : public Mithril::Application
{
public:
    Sandbox();
    ~Sandbox();
private:
    std::shared_ptr<SandboxLayer> m_Layer;
};
