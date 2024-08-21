#include "Renderer.hpp"

namespace Mithril {

    Renderer::Renderer()
    {
        m_Context = CreateRef<VulkanContext>();
        m_Surface = CreateRef<VulkanSurface>(m_Context);
        m_Device = CreateRef<VulkanDevice>(m_Context, m_Surface);
        m_Swapchain = CreateRef<VulkanSwapchain>(m_Surface, m_Device);
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
