#pragma once
#include "Mithril/Defines.hpp"

#include <vector>

#include <volk.h>

#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanRenderPass.hpp"

namespace Mithril {

    class VulkanFramebuffer
    {
    public:
        VulkanFramebuffer(const Ref<VulkanDevice>& device, const Ref<VulkanSwapchain>& swapchain, const Ref<VulkanRenderPass>& renderPass);
        ~VulkanFramebuffer();

        inline VkFramebuffer Framebuffer(u32 index) const { return m_Framebuffers[index]; }

    private:
        Ref<VulkanDevice> m_Device;
        Ref<VulkanSwapchain> m_Swapchain;
        Ref<VulkanRenderPass> m_RenderPass;

        std::vector<VkFramebuffer> m_Framebuffers;
    };

}
