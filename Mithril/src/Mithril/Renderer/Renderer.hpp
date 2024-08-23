#pragma once
#include "Mithril/Defines.hpp"

#include <vector>
#include <optional>
#include <string>

#include <volk.h>

#include <Mithril/Core/Window.hpp>

namespace Mithril {

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void Draw();
        void Resize(u32 width, u32 height);
    
    private:
        struct QueueFamily
        {
            VkQueue Queue;
            std::optional<u32> Index;
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
        void CreateGraphicsPipeline();
        void CreateCommandPool();
        void AllocateCommandBuffer();
        void CreateSyncObjects();
        void RecordCommandBuffer(VkCommandBuffer commandBuffer, u32 index);
        void RecreateSwapchain();

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
        VkPipelineLayout m_GraphicsPipelineLayout;
        VkPipeline m_GraphicsPipeline;
        VkCommandPool m_CommandPool;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;

#ifndef NDEBUG
        VkDebugUtilsMessengerEXT m_DebugMessenger;
#endif
    };

}
