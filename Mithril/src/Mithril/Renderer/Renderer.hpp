#pragma once
#include "Mithril/Defines.hpp"

#include "Vulkan/VulkanContext.hpp"
#include "Vulkan/VulkanSurface.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanSwapchain.hpp"
#include "Vulkan/VulkanRenderPass.hpp"
#include "Vulkan/VulkanGraphicsPipeline.hpp"
#include "Vulkan/VulkanFramebuffer.hpp"
#include "Vulkan/VulkanCommandPool.hpp"

namespace Mithril {

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void Draw();

        void Resize(u32 width, u32 height);

        void Submit(VkCommandBuffer commandBuffer, u32 imageIndex);
        void CreateSyncObjects();
        void DestroySyncObjects();

    private:
        static constexpr u32 MAX_FRAMES_IN_FLIGHT = 2;

        Ref<VulkanContext> m_Context;
        Ref<VulkanSurface> m_Surface;
        Ref<VulkanDevice> m_Device;
        Ref<VulkanSwapchain> m_Swapchain;
        Ref<VulkanRenderPass> m_RenderPass;
        Ref<VulkanGraphicsPipeline> m_GraphicsPipeline;
        Ref<VulkanFramebuffer> m_Framebuffer;
        Ref<VulkanCommandPool> m_CommandPool;

        std::vector<VkSemaphore> m_ImageAvailableSemaphore;
        std::vector<VkSemaphore> m_RenderFinishedSemaphore;
        std::vector<VkFence> m_InFlightFence;

        u32 m_CurrentFrame { 0 };
    };

}
