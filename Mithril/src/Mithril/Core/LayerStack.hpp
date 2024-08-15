#pragma once

#include <vector>
#include <memory>

#include "Layer.hpp"

namespace Mithril {

    class LayerStack
    {
    public:
        LayerStack() = default;
        ~LayerStack();

        void PushLayer(const std::shared_ptr<Layer>& layer);
        void PushOverlay(const std::shared_ptr<Layer>& overlay);
        void PopLayer(const std::shared_ptr<Layer>& layer);
        void PopOverlay(const std::shared_ptr<Layer>& overlay);

        auto begin() { return m_Stack.begin(); }
        auto end() { return m_Stack.end(); }
        auto rbegin() { return m_Stack.rbegin(); }
        auto rend() { return m_Stack.rend(); }

        auto begin() const { return m_Stack.begin(); }
        auto end() const { return m_Stack.end(); }
        auto rbegin() const { return m_Stack.rbegin(); }
        auto rend() const { return m_Stack.rend(); }

    private:
        std::vector<std::shared_ptr<Layer>> m_Stack;
        uint32_t m_Index { 0 };
    };

}
