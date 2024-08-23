#pragma once
#include "Mithril/Defines.hpp"

#include <string>
#include <array>

#include <volk.h>
#include <glm/glm.hpp>

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

        inline VkPipeline Pipeline() const { return m_Pipeline; }

    private:
        VkShaderModule CompileShaderFile(const std::string& filepath);

    private:
        Ref<VulkanDevice> m_Device;
        Ref<VulkanSwapchain> m_Swapchain;
        Ref<VulkanRenderPass> m_RenderPass;

        VkPipelineLayout m_Layout;
        VkPipeline m_Pipeline;

        struct Vertex
        {
            glm::vec2 Pos;
            glm::vec3 Color;

            static VkVertexInputBindingDescription GetBindingDescription()
            {
                VkVertexInputBindingDescription bindingDescription;
                bindingDescription.binding = 0;
                bindingDescription.stride = sizeof(Vertex);
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                return bindingDescription;
            }

            static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
            {
                std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions;

                attributeDescriptions[0].location = 0;
                attributeDescriptions[0].binding = 0;
                attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
                attributeDescriptions[0].offset = offsetof(Vertex, Pos);

                attributeDescriptions[1].location = 1;
                attributeDescriptions[1].binding = 0;
                attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[1].offset = offsetof(Vertex, Color);

                return attributeDescriptions;
            }
        };
    };

}
