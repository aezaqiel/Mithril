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

        inline VkSwapchainKHR Swapchain() const { return m_Swapchain; }
        inline VkSurfaceFormatKHR SurfaceFormat() const { return m_SurfaceFormat; }
        inline VkExtent2D Extent() const { return m_Extent; }
        inline u32 ImageCount() const { return m_Images.size(); }
        inline VkImageView ImageView(u32 index) const { return m_ImageViews[index]; }

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
        void CreateImageViews();

    private:
        Ref<VulkanSurface> m_Surface;
        Ref<VulkanDevice> m_Device;

        VkSurfaceFormatKHR m_SurfaceFormat;
        VkPresentModeKHR m_PresentMode;
        VkExtent2D m_Extent;

        VkSwapchainKHR m_Swapchain;
        std::vector<VkImage> m_Images;
        std::vector<VkImageView> m_ImageViews;
    };

}

