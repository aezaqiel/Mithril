#pragma once
#include "Mithril/Defines.hpp"

#include <volk.h>

#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"

namespace Mithril {

    class VulkanRenderPass
    {
    public:
        VulkanRenderPass(const Ref<VulkanDevice>& device, const Ref<VulkanSwapchain>& swapchain);
        ~VulkanRenderPass();

        inline VkRenderPass RenderPass() const { return m_RenderPass; }

    private:
        Ref<VulkanDevice> m_Device;
        Ref<VulkanSwapchain> m_Swapchain;

        VkRenderPass m_RenderPass;
    };

}
