#pragma once
#include "Mithril/Defines.hpp"

#include <vector>
#include <optional>

#include "VulkanContext.hpp"
#include "VulkanSurface.hpp"

namespace Mithril {

    class VulkanDevice
    {
    public:
        VulkanDevice(const Ref<VulkanContext>& context, const Ref<VulkanSurface>& surface);
        ~VulkanDevice();

        inline VkDevice Device() const { return m_Device; }
        inline VkQueue GraphicsQueue() const { return m_GraphicsQueue; }
        inline VkQueue PresentQueue() const { return m_PresentQueue; }
    
    private:
        struct QueueFamilyIndices
        {
            std::optional<u32> GraphicsFamily;
            std::optional<u32> PresentFamily;

            bool Complete() const { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
        };

    private:
        bool PhysicalDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices FindQueueFamilyIndices(VkPhysicalDevice device);

        void CheckExtensions();

    private:
        Ref<VulkanContext> m_Context;
        Ref<VulkanSurface> m_Surface;

        std::vector<const char*> m_Extensions;
        VkPhysicalDevice m_PhysicalDevice;
        VkDevice m_Device;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;
    };

}
