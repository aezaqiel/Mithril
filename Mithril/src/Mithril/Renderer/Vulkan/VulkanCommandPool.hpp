#pragma once
#include "Mithril/Defines.hpp"

#include <vector>

#include <volk.h>

#include "VulkanDevice.hpp"

namespace Mithril {

    class VulkanCommandPool
    {
    public:
        VulkanCommandPool(const Ref<VulkanDevice>& device);
        ~VulkanCommandPool();

        void AllocateCommandBuffer(VkCommandBufferLevel level, u32 count);

        inline VkCommandBuffer CommandBuffer(u32 index) const { return m_CommandBuffer[index]; }

    private:
        Ref<VulkanDevice> m_Device;

        VkCommandPool m_CommandPool;
        std::vector<VkCommandBuffer> m_CommandBuffer;
    };

}
