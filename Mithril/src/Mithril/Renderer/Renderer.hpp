#pragma once
#include "Mithril/Defines.hpp"

#include <vector>
#include <optional>
#include <string>
#include <array>

#include <volk.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Mithril/Core/Window.hpp>

namespace Mithril {

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void Draw(f32 dt);
        void Resize(u32 width, u32 height);
    
    private:
        struct QueueFamily
        {
            VkQueue Queue;
            std::optional<u32> Index;
        };

        struct Vertex
        {
            glm::vec2 Pos;
            glm::vec3 Color;

            static VkVertexInputBindingDescription GetBindingDescription()
            {
                VkVertexInputBindingDescription description;
                description.binding = 0;
                description.stride = sizeof(Vertex);
                description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                return description;
            }

            static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
            {
                std::array<VkVertexInputAttributeDescription, 2> attributes;
                attributes[0].location = 0;
                attributes[0].binding = 0;
                attributes[0].format = VK_FORMAT_R32G32_SFLOAT;
                attributes[0].offset = offsetof(Vertex, Pos);

                attributes[1].location = 1;
                attributes[1].binding = 0;
                attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributes[1].offset = offsetof(Vertex, Color);

                return attributes;
            }
        };

        struct UniformBufferObject
        {
            glm::mat4 Model;
            glm::mat4 View;
            glm::mat4 Proj;
        };

    private:
        void CreateInstance();
        void CreateSurface();
        bool DeviceSuitable(const VkPhysicalDevice& device);
        void PickPhysicalDevice();
        void CreateDevice();
        void QuerySurfaceCapabilities();
        void SetExtent(u32 width, u32 height);
        void CreateRenderPass();
        void CreateSwapchain();
        void CreateFramebuffers();
        VkShaderModule CompileShaderFile(const std::string& filepath);
        void CreateDescriptorSetLayout();
        void CreateGraphicsPipeline();
        void CreateCommandPool();
        u32 FindMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties);
        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void CreateUniformBuffers();
        void CreateDescriptorPool();
        void CreateDescriptorSets();
        void AllocateCommandBuffer();
        void CreateSyncObjects();
        void RecordCommandBuffer(VkCommandBuffer commandBuffer, u32 index);
        void RecreateSwapchain();
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void UpdateUniformBuffer(u32 currentImage, f32 dt);

    private:
        static constexpr u32 MAX_FRAMES_IN_FLIGHT { 2 };
        u32 m_CurrentFrame { 0 };

        VkInstance m_Instance;
        VkSurfaceKHR m_Surface;
        VkPhysicalDevice m_PhysicalDevice;
        VkDevice m_Device;
        QueueFamily m_GraphicsQueue;
        QueueFamily m_PresentQueue;
        VkSurfaceFormatKHR m_SurfaceFormat;
        VkPresentModeKHR m_PresentMode;
        VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;
        VkExtent2D m_Extent;
        VkRenderPass m_RenderPass;
        VkSwapchainKHR m_Swapchain;
        std::vector<VkImage> m_Images;
        std::vector<VkImageView> m_ImageViews;
        std::vector<VkFramebuffer> m_Framebuffers;
        VkDescriptorSetLayout m_DescriptorSetLayout;
        VkPipelineLayout m_GraphicsPipelineLayout;
        VkPipeline m_GraphicsPipeline;
        VkCommandPool m_CommandPool;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;
        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;
        VkBuffer m_IndexBuffer;
        VkDeviceMemory m_IndexBufferMemory;
        std::vector<VkBuffer> m_UniformBuffers;
        std::vector<VkDeviceMemory> m_UniformBuffersMemory;
        std::vector<void*> m_UniformBuffersMapped;
        VkDescriptorPool m_DescriptorPool;
        std::vector<VkDescriptorSet> m_DescriptorSets;

        const std::vector<Vertex> m_Vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}
        };

        const std::vector<u16> m_Indices = {
            0, 1, 2, 2, 3, 0
        };

#ifndef NDEBUG
        VkDebugUtilsMessengerEXT m_DebugMessenger;
#endif
    };

}
