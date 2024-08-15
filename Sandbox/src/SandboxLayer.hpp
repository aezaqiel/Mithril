#pragma once

#include <Mithril/Mithril.hpp>

class SandboxLayer : public Mithril::Layer
{
public:
    SandboxLayer(const std::string& name = "SandboxLayer")
        : Layer(name) {}

    virtual ~SandboxLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float) override;
    virtual void OnEvent(Mithril::Event&) override;

private:
};
