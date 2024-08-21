#pragma once
#include "Mithril/Defines.hpp"

#include <vector>

#include <volk.h>

#include "VulkanSurface.hpp"
#include "VulkanDevice.hpp"

namespace Mithril {

    class VulkanSwapchain
    {
    public:
        VulkanSwapchain(const Ref<VulkanSurface>& surface, const Ref<VulkanDevice>& device);
        ~VulkanSwapchain();

    private:
        struct SwapchainSupportDetails
        {
            VkSurfaceCapabilitiesKHR Capabilities;
            std::vector<VkSurfaceFormatKHR> Formats;
            std::vector<VkPresentModeKHR> PresentModes;
        };

    private:
        SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availableModes);
        VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    private:
        Ref<VulkanSurface> m_Surface;
        Ref<VulkanDevice> m_Device;

        VkSurfaceFormatKHR m_SurfaceFormat;
        VkPresentModeKHR m_PresentMode;
        VkExtent2D m_Extent;

        VkSwapchainKHR m_Swapchain;
        std::vector<VkImage> m_Images;
    };

}

