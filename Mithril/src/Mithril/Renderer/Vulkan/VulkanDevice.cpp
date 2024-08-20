#include "VulkanDevice.hpp"

#include <set>

#include "Mithril/Core/Logger.hpp"

namespace Mithril {

    VulkanDevice::VulkanDevice(const Ref<VulkanContext>& context, const Ref<VulkanSurface>& surface)
        : m_Context(context), m_Surface(surface)
    {
        uint32_t availablePhysicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(m_Context->Instance(), &availablePhysicalDeviceCount, nullptr);

        if (availablePhysicalDeviceCount == 0) {
            M_CORE_ERROR("Failed to find GPUs with vulkan support");
        }

        std::vector<VkPhysicalDevice> availablePhysicalDevices(availablePhysicalDeviceCount);
        vkEnumeratePhysicalDevices(m_Context->Instance(), &availablePhysicalDeviceCount, availablePhysicalDevices.data());

        for (const auto& device : availablePhysicalDevices) {
            if (PhysicalDeviceSuitable(device)) {
                m_PhysicalDevice = device;
                break;
            }
        }

        if (m_PhysicalDevice == VK_NULL_HANDLE) {
            M_CORE_ERROR("No physical device supports all required features & properties");
        } else {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);

            M_CORE_DEBUG("Using physical device {}", properties.deviceName);
        }

        QueueFamilyIndices indices = FindQueueFamilyIndices(m_PhysicalDevice);

        std::set<uint32_t> queueFamilyIndices = {
            indices.GraphicsFamily.value(),
            indices.PresentFamily.value()
        };

        float queueFamilyPriority[] = { 1.0f };

        std::vector<VkDeviceQueueCreateInfo> queueInfos;
        for (uint32_t family : queueFamilyIndices) {
            VkDeviceQueueCreateInfo queueInfo;
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.pNext = nullptr;
            queueInfo.flags = 0;
            queueInfo.queueFamilyIndex = family;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = queueFamilyPriority;

            queueInfos.push_back(queueInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures;
        deviceFeatures.robustBufferAccess = VK_FALSE;
        deviceFeatures.fullDrawIndexUint32 = VK_FALSE;
        deviceFeatures.imageCubeArray = VK_FALSE;
        deviceFeatures.independentBlend = VK_FALSE;
        deviceFeatures.geometryShader = VK_FALSE;
        deviceFeatures.tessellationShader = VK_FALSE;
        deviceFeatures.sampleRateShading = VK_FALSE;
        deviceFeatures.dualSrcBlend = VK_FALSE;
        deviceFeatures.logicOp = VK_FALSE;
        deviceFeatures.multiDrawIndirect = VK_FALSE;
        deviceFeatures.drawIndirectFirstInstance = VK_FALSE;
        deviceFeatures.depthClamp = VK_FALSE;
        deviceFeatures.depthBiasClamp = VK_FALSE;
        deviceFeatures.fillModeNonSolid = VK_FALSE;
        deviceFeatures.depthBounds = VK_FALSE;
        deviceFeatures.wideLines = VK_FALSE;
        deviceFeatures.largePoints = VK_FALSE;
        deviceFeatures.alphaToOne = VK_FALSE;
        deviceFeatures.multiViewport = VK_FALSE;
        deviceFeatures.samplerAnisotropy = VK_FALSE;
        deviceFeatures.textureCompressionETC2 = VK_FALSE;
        deviceFeatures.textureCompressionASTC_LDR = VK_FALSE;
        deviceFeatures.textureCompressionBC = VK_FALSE;
        deviceFeatures.occlusionQueryPrecise = VK_FALSE;
        deviceFeatures.pipelineStatisticsQuery = VK_FALSE;
        deviceFeatures.vertexPipelineStoresAndAtomics = VK_FALSE;
        deviceFeatures.fragmentStoresAndAtomics = VK_FALSE;
        deviceFeatures.shaderTessellationAndGeometryPointSize = VK_FALSE;
        deviceFeatures.shaderImageGatherExtended = VK_FALSE;
        deviceFeatures.shaderStorageImageExtendedFormats = VK_FALSE;
        deviceFeatures.shaderStorageImageMultisample = VK_FALSE;
        deviceFeatures.shaderStorageImageReadWithoutFormat = VK_FALSE;
        deviceFeatures.shaderStorageImageWriteWithoutFormat = VK_FALSE;
        deviceFeatures.shaderUniformBufferArrayDynamicIndexing = VK_FALSE;
        deviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_FALSE;
        deviceFeatures.shaderStorageBufferArrayDynamicIndexing = VK_FALSE;
        deviceFeatures.shaderStorageImageArrayDynamicIndexing = VK_FALSE;
        deviceFeatures.shaderClipDistance = VK_FALSE;
        deviceFeatures.shaderCullDistance = VK_FALSE;
        deviceFeatures.shaderFloat64 = VK_FALSE;
        deviceFeatures.shaderInt64 = VK_FALSE;
        deviceFeatures.shaderInt16 = VK_FALSE;
        deviceFeatures.shaderResourceResidency = VK_FALSE;
        deviceFeatures.shaderResourceMinLod = VK_FALSE;
        deviceFeatures.sparseBinding = VK_FALSE;
        deviceFeatures.sparseResidencyBuffer = VK_FALSE;
        deviceFeatures.sparseResidencyImage2D = VK_FALSE;
        deviceFeatures.sparseResidencyImage3D = VK_FALSE;
        deviceFeatures.sparseResidency2Samples = VK_FALSE;
        deviceFeatures.sparseResidency4Samples = VK_FALSE;
        deviceFeatures.sparseResidency8Samples = VK_FALSE;
        deviceFeatures.sparseResidency16Samples = VK_FALSE;
        deviceFeatures.sparseResidencyAliased = VK_FALSE;
        deviceFeatures.variableMultisampleRate = VK_FALSE;
        deviceFeatures.inheritedQueries = VK_FALSE;

        m_Extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        CheckExtensions();

        VkDeviceCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.queueCreateInfoCount = queueInfos.size();
        createInfo.pQueueCreateInfos = queueInfos.data();
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.enabledExtensionCount = m_Extensions.size();
        createInfo.ppEnabledExtensionNames = m_Extensions.data();
        createInfo.pEnabledFeatures = &deviceFeatures;

        if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create logical device");
        }

        volkLoadDevice(m_Device);

        vkGetDeviceQueue(m_Device, indices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, indices.PresentFamily.value(), 0, &m_PresentQueue);
    }

    VulkanDevice::~VulkanDevice()
    {
        vkDestroyDevice(m_Device, nullptr);
    }

    bool VulkanDevice::PhysicalDeviceSuitable(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        M_CORE_DEBUG("Checking physical device {}", properties.deviceName);

        if (properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            M_CORE_TRACE("Not a discrete gpu");
            return false;
        }

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        if (!features.geometryShader) {
            M_CORE_TRACE("Geometry shader not supported");
            return false;
        }

        QueueFamilyIndices indices = FindQueueFamilyIndices(device);

        if (!indices.Complete()) {
            M_CORE_TRACE("Required queue families not supported");
            return false;
        }

        return true;
    }

    VulkanDevice::QueueFamilyIndices VulkanDevice::FindQueueFamilyIndices(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, queueFamilies.data());

        int32_t i = 0;
        for (const auto& family: queueFamilies) {
            if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.GraphicsFamily = i;
            }

            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface->Surface(), &presentSupport);

            if (presentSupport == VK_TRUE) {
                indices.PresentFamily = i;
            }

            i++;
        }

        return indices;
    }

    void VulkanDevice::CheckExtensions()
    {
        u32 count = 0;
        vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> extensions(count);
        vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &count, extensions.data());

        for (const auto& requested : m_Extensions) {
            bool found = false;
            for (const auto& available : extensions) {
                if (strcmp(available.extensionName, requested) == 0) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                M_CORE_ERROR("Requested device extension {} not available", requested);
            }
        }
    }

}
