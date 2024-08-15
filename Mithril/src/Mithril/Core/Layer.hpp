#pragma once

#include <string>

#include "Events/Event.hpp"

namespace Mithril {

    class Layer
    {
    public:
        Layer(const std::string& name = "Layer")
            : m_Name(name) {}

        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(float) {}
        virtual void OnEvent(Event&) {}

        const std::string& Name() const { return m_Name; }

    private:
        std::string m_Name;
    };

}
