#include "VulkanSwapchain.hpp"

#include <limits>
#include <algorithm>

#include "Mithril/Core/Logger.hpp"
#include "Mithril/Core/Application.hpp"

namespace Mithril {

    VulkanSwapchain::VulkanSwapchain(const Ref<VulkanSurface>& surface, const Ref<VulkanDevice>& device)
        : m_Surface(surface), m_Device(device)
    {
        SwapchainSupportDetails details = QuerySwapchainSupport(m_Device->PhysicalDevice());

        m_SurfaceFormat = ChooseSurfaceFormat(details.Formats);
        m_PresentMode = ChoosePresentMode(details.PresentModes);
        m_Extent = ChooseExtent(details.Capabilities);

        u32 imageCount = details.Capabilities.minImageCount + 1;
        if (details.Capabilities.maxImageCount > 0 && imageCount > details.Capabilities.maxImageCount) {
            imageCount = details.Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.surface = m_Surface->Surface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = m_SurfaceFormat.format;
        createInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
        createInfo.imageExtent = m_Extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.preTransform = details.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = m_PresentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        u32 queueFamilyIndices[] = {
            m_Device->GraphicsQueue().Index,
            m_Device->PresentQueue().Index
        };

        if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        if (vkCreateSwapchainKHR(m_Device->Device(), &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create swapchain");
        }

        vkGetSwapchainImagesKHR(m_Device->Device(), m_Swapchain, &imageCount, nullptr);
        m_Images.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Device->Device(), m_Swapchain, &imageCount, m_Images.data());

        CreateImageViews();
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        for (auto& imageView : m_ImageViews) {
            vkDestroyImageView(m_Device->Device(), imageView, nullptr);
        }

        vkDestroySwapchainKHR(m_Device->Device(), m_Swapchain, nullptr);
    }

    VulkanSwapchain::SwapchainSupportDetails VulkanSwapchain::QuerySwapchainSupport(VkPhysicalDevice device)
    {
        SwapchainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface->Surface(), &details.Capabilities);

        u32 formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface->Surface(), &formatCount, nullptr);
        if (formatCount > 0) {
            details.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface->Surface(), &formatCount, details.Formats.data());
        }

        u32 presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface->Surface(), &presentModeCount, nullptr);
        if (presentModeCount > 0) {
            details.PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface->Surface(), &presentModeCount, details.PresentModes.data());
        }

        if (details.Formats.empty() || details.PresentModes.empty()) {
            M_CORE_ERROR("Device does not have adequate support for swapchain");
        }

        return details;
    }

    VkSurfaceFormatKHR VulkanSwapchain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& format : availableFormats) {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return format;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR VulkanSwapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& availableModes)
    {
        for (const auto& mode : availableModes) {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return mode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanSwapchain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<u32>::max()) {
            return capabilities.currentExtent;
        } else {
            u32 width = Application::Instance().GetWindow()->Width();
            u32 height = Application::Instance().GetWindow()->Height();

            width = std::clamp(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            height = std::clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            VkExtent2D actualExtent = { width, height };

            return actualExtent;
        }
    }

    void VulkanSwapchain::CreateImageViews()
    {
        m_ImageViews.resize(m_Images.size());
        for (u32 i = 0; i < m_Images.size(); i++) {
            VkImageViewCreateInfo createInfo;
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.pNext = nullptr;
            createInfo.flags = 0;
            createInfo.image = m_Images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_SurfaceFormat.format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_Device->Device(), &createInfo, nullptr, &m_ImageViews[i]) != VK_SUCCESS) {
                M_CORE_ERROR("Failed to create image view, index {}", i);
            }
        }
    }

}
