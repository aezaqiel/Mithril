#include "VulkanCommandPool.hpp"

#include "Mithril/Core/Logger.hpp"

namespace Mithril {

    VulkanCommandPool::VulkanCommandPool(const Ref<VulkanDevice>& device)
        : m_Device(device)
    {
        VkCommandPoolCreateInfo poolInfo;
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.pNext = nullptr;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = m_Device->GraphicsQueue().Index;

        if (vkCreateCommandPool(m_Device->Device(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create command pool");
        }
    }

    VulkanCommandPool::~VulkanCommandPool()
    {
        vkDestroyCommandPool(m_Device->Device(), m_CommandPool, nullptr);
    }

    void VulkanCommandPool::AllocateCommandBuffer(VkCommandBufferLevel level, u32 count)
    {
        m_CommandBuffer.resize(count);

        VkCommandBufferAllocateInfo allocInfo;
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = level;
        allocInfo.commandBufferCount = count;

        if (vkAllocateCommandBuffers(m_Device->Device(), &allocInfo, m_CommandBuffer.data()) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to allocate command buffer");
        }
    }

}
