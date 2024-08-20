#include "LayerStack.hpp"

namespace Mithril {

    LayerStack::~LayerStack()
    {
        for (const auto& layer : m_Stack) {
            layer->OnDetach();
        }
    }

    void LayerStack::PushLayer(const Ref<Layer>& layer)
    {
        m_Stack.emplace(m_Stack.begin() + m_Index, std::move(layer));
        m_Index++;
    }

    void LayerStack::PushOverlay(const Ref<Layer>& overlay)
    {
        m_Stack.emplace_back(std::move(overlay));
    }

    void LayerStack::PopLayer(const Ref<Layer>& layer)
    {
        auto it = std::find(m_Stack.begin(), m_Stack.begin() + m_Index, layer);
        if (it != m_Stack.end()) {
            m_Stack.erase(it);
            m_Index--;
        }
    }

    void LayerStack::PopOverlay(const Ref<Layer>& overlay)
    {
        auto it = std::find(m_Stack.begin() + m_Index, m_Stack.end(), overlay);
        if (it != m_Stack.end()) {
            m_Stack.erase(it);
        }
    }

}
