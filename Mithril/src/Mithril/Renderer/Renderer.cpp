#include "Renderer.hpp"

namespace Mithril {

    Renderer::Renderer()
    {
        m_Context = CreateRef<VulkanContext>();
        m_Surface = CreateRef<VulkanSurface>(m_Context);
        m_Device = CreateRef<VulkanDevice>(m_Context, m_Surface);
        m_Swapchain = CreateRef<VulkanSwapchain>(m_Surface, m_Device);
        m_RenderPass = CreateRef<VulkanRenderPass>(m_Device, m_Swapchain);
        m_GraphicsPipeline = CreateRef<VulkanGraphicsPipeline>(
            m_Device,
            m_Swapchain,
            m_RenderPass,
            ".\\Shaders\\Triangle.vert.spv",
            ".\\Shaders\\Triangle.frag.spv"
        );
        m_Framebuffer = CreateRef<VulkanFramebuffer>(m_Device, m_Swapchain, m_RenderPass);
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
