#pragma once
#include "Mithril/Defines.hpp"

#include <vector>

#include "Layer.hpp"

namespace Mithril {

    class LayerStack
    {
    public:
        LayerStack() = default;
        ~LayerStack();

        void PushLayer(const Ref<Layer>& layer);
        void PushOverlay(const Ref<Layer>& overlay);
        void PopLayer(const Ref<Layer>& layer);
        void PopOverlay(const Ref<Layer>& overlay);

        auto begin() { return m_Stack.begin(); }
        auto end() { return m_Stack.end(); }
        auto rbegin() { return m_Stack.rbegin(); }
        auto rend() { return m_Stack.rend(); }

        auto begin() const { return m_Stack.begin(); }
        auto end() const { return m_Stack.end(); }
        auto rbegin() const { return m_Stack.rbegin(); }
        auto rend() const { return m_Stack.rend(); }

    private:
        std::vector<Ref<Layer>> m_Stack;
        u32 m_Index { 0 };
    };

}
