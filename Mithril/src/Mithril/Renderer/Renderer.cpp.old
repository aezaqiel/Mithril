#include "Renderer.hpp"

#include <limits>

#include "Mithril/Core/Logger.hpp"
#include "vulkan/vulkan_core.h"

namespace Mithril {

    Renderer::Renderer()
    {
        m_Context = CreateRef<VulkanContext>();
        m_Surface = CreateRef<VulkanSurface>(m_Context);
        m_Device = CreateRef<VulkanDevice>(m_Context, m_Surface);
        m_Swapchain = CreateRef<VulkanSwapchain>(m_Surface, m_Device);
        m_RenderPass = CreateRef<VulkanRenderPass>(m_Device, m_Swapchain);
        m_Framebuffer = CreateRef<VulkanFramebuffer>(m_Device, m_Swapchain, m_RenderPass);
        m_GraphicsPipeline = CreateRef<VulkanGraphicsPipeline>(
            m_Device,
            m_Swapchain,
            m_RenderPass,
            ".\\Shaders\\Triangle.vert.spv",
            ".\\Shaders\\Triangle.frag.spv"
        );
        m_CommandPool = CreateRef<VulkanCommandPool>(m_Device);
        m_CommandPool->AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, MAX_FRAMES_IN_FLIGHT);

        CreateSyncObjects();
    }

    Renderer::~Renderer()
    {
        vkDeviceWaitIdle(m_Device->Device());
        DestroySyncObjects();
    }

    void Renderer::Draw()
    {
        vkWaitForFences(m_Device->Device(), 1, &m_InFlightFence[m_CurrentFrame], VK_TRUE, std::numeric_limits<u64>::max());
        vkResetFences(m_Device->Device(), 1, &m_InFlightFence[m_CurrentFrame]);

        u32 imageIndex;
        vkAcquireNextImageKHR(m_Device->Device(), m_Swapchain->Swapchain(), std::numeric_limits<u64>::max(), m_ImageAvailableSemaphore[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);

        vkResetCommandBuffer(m_CommandPool->CommandBuffer(m_CurrentFrame), 0);

        Submit(m_CommandPool->CommandBuffer(m_CurrentFrame), imageIndex);

        VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphore[m_CurrentFrame] };
        VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphore[m_CurrentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkCommandBuffer commandBuffer = m_CommandPool->CommandBuffer(m_CurrentFrame);
        VkSwapchainKHR swapchain = m_Swapchain->Swapchain();

        VkSubmitInfo submitInfo;
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(m_Device->GraphicsQueue().Queue, 1, &submitInfo, m_InFlightFence[m_CurrentFrame]) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to submit draw command buffer");
        }

        VkPresentInfoKHR presentInfo;
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;

        vkQueuePresentKHR(m_Device->PresentQueue().Queue, &presentInfo);

        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::Resize(u32 width, u32 height)
    {
        (void)width;
        (void)height;

        vkDeviceWaitIdle(m_Device->Device());

        m_GraphicsPipeline.reset();
        m_Framebuffer.reset();
        m_RenderPass.reset();
        m_Swapchain.reset();

        m_Swapchain = CreateRef<VulkanSwapchain>(m_Surface, m_Device);
        m_RenderPass = CreateRef<VulkanRenderPass>(m_Device, m_Swapchain);
        m_Framebuffer = CreateRef<VulkanFramebuffer>(m_Device, m_Swapchain, m_RenderPass);
        m_GraphicsPipeline = CreateRef<VulkanGraphicsPipeline>(
            m_Device,
            m_Swapchain,
            m_RenderPass,
            ".\\Shaders\\Triangle.vert.spv",
            ".\\Shaders\\Triangle.frag.spv"
        );
    }

    void Renderer::Submit(VkCommandBuffer commandBuffer, u32 imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo;
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = nullptr;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to begin recording command buffer");
        }

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

        VkRenderPassBeginInfo renderPassBeginInfo;
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.pNext = nullptr;
        renderPassBeginInfo.renderPass = m_RenderPass->RenderPass();
        renderPassBeginInfo.framebuffer = m_Framebuffer->Framebuffer(imageIndex);
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = m_Swapchain->Extent();
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->Pipeline());

        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = m_Swapchain->Extent().width;
        viewport.height = m_Swapchain->Extent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor;
        scissor.offset = { 0, 0 };
        scissor.extent = m_Swapchain->Extent();

        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to record command buffer");
        }
    }

    void Renderer::CreateSyncObjects()
    {
        m_ImageAvailableSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
        m_InFlightFence.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo;
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreInfo.pNext = nullptr;
        semaphoreInfo.flags = 0;

        VkFenceCreateInfo fenceInfo;
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = nullptr;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            if (vkCreateSemaphore(m_Device->Device(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore[i]) != VK_SUCCESS) {
                M_CORE_ERROR("Failed to create image available semaphore");
            }

            if (vkCreateSemaphore(m_Device->Device(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore[i]) != VK_SUCCESS) {
                M_CORE_ERROR("Failed to create image available semaphore");
            }

            if (vkCreateFence(m_Device->Device(), &fenceInfo, nullptr, &m_InFlightFence[i]) != VK_SUCCESS) {
                M_CORE_ERROR("Failed to create fence");
            }
        }
    }

    void Renderer::DestroySyncObjects()
    {
        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            vkDestroyFence(m_Device->Device(), m_InFlightFence[i], nullptr);
            vkDestroySemaphore(m_Device->Device(), m_RenderFinishedSemaphore[i], nullptr);
            vkDestroySemaphore(m_Device->Device(), m_ImageAvailableSemaphore[i], nullptr);
        }
    }

}
