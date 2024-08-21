#include "VulkanFramebuffer.hpp"

#include "Mithril/Core/Logger.hpp"

namespace Mithril {

    VulkanFramebuffer::VulkanFramebuffer(const Ref<VulkanDevice>& device, const Ref<VulkanSwapchain>& swapchain, const Ref<VulkanRenderPass>& renderPass)
        : m_Device(device), m_Swapchain(swapchain), m_RenderPass(renderPass)
    {
        m_Framebuffers.resize(m_Swapchain->ImageCount());

        for (u32 i = 0; i < m_Swapchain->ImageCount(); ++i) {
            VkImageView attachments[] = {
                m_Swapchain->ImageView(i),
            };

            VkFramebufferCreateInfo createInfo;
            createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            createInfo.pNext = nullptr;
            createInfo.flags = 0;
            createInfo.renderPass = m_RenderPass->RenderPass();
            createInfo.attachmentCount = 1;
            createInfo.pAttachments = attachments;
            createInfo.width = m_Swapchain->Extent().width;
            createInfo.height = m_Swapchain->Extent().height;
            createInfo.layers = 1;

            if (vkCreateFramebuffer(m_Device->Device(), &createInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS) {
                M_CORE_ERROR("Failed to create framebuffer, index: {}", i);
            }
        }
    }

    VulkanFramebuffer::~VulkanFramebuffer()
    {
        for (auto framebuffer : m_Framebuffers) {
            vkDestroyFramebuffer(m_Device->Device(), framebuffer, nullptr);
        }
    }

}
