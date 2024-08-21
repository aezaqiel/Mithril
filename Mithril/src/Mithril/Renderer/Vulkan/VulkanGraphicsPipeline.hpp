#pragma once
#include "Mithril/Defines.hpp"

#include <string>

#include <volk.h>

#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanRenderPass.hpp"

namespace Mithril {

    class VulkanGraphicsPipeline
    {
    public:
        VulkanGraphicsPipeline(
            const Ref<VulkanDevice>& device,
            const Ref<VulkanSwapchain>& swapchain,
            const Ref<VulkanRenderPass>& renderPass,
            const std::string& vertexShaderPath,
            const std::string& fragmentShaderPath
        );

        ~VulkanGraphicsPipeline();

    private:
        VkShaderModule CompileShaderFile(const std::string& filepath);

    private:
        Ref<VulkanDevice> m_Device;
        Ref<VulkanSwapchain> m_Swapchain;
        Ref<VulkanRenderPass> m_RenderPass;

        VkPipelineLayout m_Layout;
        VkPipeline m_Pipeline;
    };

}
