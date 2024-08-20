#include "Renderer.hpp"

namespace Mithril {

    Renderer::Renderer()
    {
        m_Context = CreateRef<VulkanContext>();
        m_Surface = CreateRef<VulkanSurface>(m_Context);
        m_Device = CreateRef<VulkanDevice>(m_Context, m_Surface);
    }

    void Renderer::Draw()
    {
    }

    void Renderer::Resize(u32 width, u32 height)
    {
        (void)width;
        (void)height;
    }

}
