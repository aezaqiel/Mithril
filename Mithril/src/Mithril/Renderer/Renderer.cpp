#include "Renderer.hpp"

#include <set>
#include <limits>
#include <algorithm>
#include <fstream>

#include "Mithril/Core/Logger.hpp"
#include "Mithril/Core/Application.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Mithril {

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
    {
        (void)userData;

        std::stringstream ss;

        switch (messageType) {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: {
                ss << "[GENERAL] ";
            } break;
            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: {
                ss << "[VALIDATION] ";
            } break;
            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: {
                ss << "[PERFORMANCE] ";
            } break;
            case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT: {
                ss << "[DEVICE ADDRESS BINDING] ";
            } break;
            default:
                ss << "[UNKNOWN] ";
        }

        switch (messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
                ss << "[VERBOSE] ";
            } break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
                ss << "[INFO] ";
            } break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
                ss << "[WARNING] ";
            } break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
                ss << "[ERROR] ";
            } break;
            default:
                ss << "[UNKOWN] ";
        }

        M_CORE_DEBUG("{} {}", ss.str(), callbackData->pMessage);

        return VK_FALSE;
    }

    Renderer::Renderer()
    {
        if (volkInitialize() != VK_SUCCESS) {
            M_CORE_ERROR("Failed to intialize vulkan loader");
        }

        CreateInstance();
        CreateSurface();
        PickPhysicalDevice();
        CreateDevice();
        QuerySurfaceCapabilities();
        SetExtent(Application::Instance().GetWindow()->Width(), Application::Instance().GetWindow()->Height());
        CreateRenderPass();
        CreateSwapchain();
        CreateGraphicsPipeline();
        CreateCommandPool();
        AllocateCommandBuffer();
        CreateSyncObjects();
    }

    Renderer::~Renderer()
    {
        vkDeviceWaitIdle(m_Device);

        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            vkDestroyFence(m_Device, m_InFlightFences[i], nullptr);
            vkDestroySemaphore(m_Device, m_RenderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[i], nullptr);
        }
        vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
        vkDestroyPipeline(m_Device, m_GraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(m_Device, m_GraphicsPipelineLayout, nullptr);
        for (auto& framebuffer : m_Framebuffers) {
            vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
        }
        for (auto& imageView : m_ImageViews) {
            vkDestroyImageView(m_Device, imageView, nullptr);
        }
        vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
        vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
        vkDestroyDevice(m_Device, nullptr);
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
#ifndef NDEBUG
        vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
#endif
        vkDestroyInstance(m_Instance, nullptr);
    }

    void Renderer::Draw()
    {
        vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

        u32 imageIndex;
        VkResult result = vkAcquireNextImageKHR(m_Device, m_Swapchain, std::numeric_limits<u64>::max(), m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            QuerySurfaceCapabilities();
            RecreateSwapchain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            M_CORE_ERROR("Failed to acquire swapchain image");
        }

        vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame]);

        vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);
        RecordCommandBuffer(m_CommandBuffers[m_CurrentFrame], imageIndex);

        VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphores[m_CurrentFrame]};
        VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_CurrentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submitInfo;
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = NULL;
        submitInfo.waitSemaphoreCount = sizeof(waitSemaphores) / sizeof(waitSemaphores[0]);
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];
        submitInfo.signalSemaphoreCount = sizeof(signalSemaphores) / sizeof(signalSemaphores[0]);
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(m_GraphicsQueue.Queue, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to submit draw command buffer");
        }

        VkSwapchainKHR swapchains[] = {m_Swapchain};

        VkPresentInfoKHR presentInfo;
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = NULL;
        presentInfo.waitSemaphoreCount = sizeof(signalSemaphores) / sizeof(signalSemaphores[0]);
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = sizeof(swapchains) / sizeof(swapchains[0]);
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = NULL;

        result = vkQueuePresentKHR(m_PresentQueue.Queue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            QuerySurfaceCapabilities();
            RecreateSwapchain();
        } else if (result != VK_SUCCESS) {
            M_CORE_ERROR("Failed to present swapchain image");
        }

        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::Resize(u32 width, u32 height)
    {
        QuerySurfaceCapabilities();
        SetExtent(width, height);
        RecreateSwapchain();
    }

    void Renderer::CreateInstance()
    {
        VkApplicationInfo info;
        info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        info.pNext = nullptr;
        info.pApplicationName = "Application";
        info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        info.pEngineName = "Mithril";
        info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        info.apiVersion = VK_API_VERSION_1_3;

        std::vector<const char*> layers;
#ifndef NDEBUG
        layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

        std::vector<const char*> extensions;
        extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

#ifndef NDEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        VkDebugUtilsMessengerCreateInfoEXT messengerInfo;
        messengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        messengerInfo.pNext = nullptr;
        messengerInfo.flags = 0;
        // messengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        messengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        messengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
        messengerInfo.pfnUserCallback = DebugCallback;
        messengerInfo.pUserData = nullptr;
#endif

        VkInstanceCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.pApplicationInfo = &info;
        createInfo.enabledLayerCount = static_cast<u32>(layers.size());
        createInfo.ppEnabledLayerNames = layers.data();
        createInfo.enabledExtensionCount = static_cast<u32>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
    
#ifndef NDEBUG
        createInfo.pNext = &messengerInfo;
#endif

        if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create vulkan instance");
        }

        volkLoadInstance(m_Instance);

#ifndef NDEBUG
        if (vkCreateDebugUtilsMessengerEXT(m_Instance, &messengerInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create debug messenger");
        }
#endif
    }

    void Renderer::CreateSurface()
    {
        if (glfwCreateWindowSurface(m_Instance, static_cast<GLFWwindow*>(Application::Instance().GetWindow()->Native()), nullptr, &m_Surface) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create surface");
        }
    }

    bool Renderer::DeviceSuitable(const VkPhysicalDevice& device)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        if (properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            return false;
        }

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        if (!features.geometryShader) {
            return false;
        }

        u32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        bool graphicsQueueAvailable = false;
        bool presentQueueAvailable = false;

        for (u32 i = 0; i < queueFamilyCount; ++i) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphicsQueueAvailable = true;
            }

            VkBool32 presentSupported = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupported);
            if (presentSupported == VK_TRUE) {
                presentQueueAvailable = true;
            }

            if (graphicsQueueAvailable && presentQueueAvailable) {
                break;
            }
        }

        if (!graphicsQueueAvailable || !presentQueueAvailable) {
            return false;
        }

        u32 formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

        if (formatCount == 0) {
            return false;
        }

        u32 presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

        if (presentModeCount == 0) {
            return false;
        }

        return true;
    }

    void Renderer::PickPhysicalDevice()
    {
        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        m_PhysicalDevice = VK_NULL_HANDLE;

        for (const auto& device : devices) {
            if (DeviceSuitable(device)) {
                m_PhysicalDevice = device;
                break;
            }
        }

        if (m_PhysicalDevice == VK_NULL_HANDLE) {
            M_CORE_ERROR("No supported physical device found");
        }

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);

        M_CORE_DEBUG("Using physical device: {}", properties.deviceName);

        u32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, queueFamilies.data());

        for (u32 i = 0; i < queueFamilyCount; ++i) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                m_GraphicsQueue.Index = i;
            }

            VkBool32 presentSupported = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Surface, &presentSupported);
            if (presentSupported == VK_TRUE) {
                m_PresentQueue.Index = i;
            }

            if (m_GraphicsQueue.Index.has_value() && m_PresentQueue.Index.has_value()) {
                break;
            }
        }
    }

    void Renderer::CreateDevice()
    {
        float queuePriority[] = { 1.0f };

        std::set<u32> queueIndices = {
            m_GraphicsQueue.Index.value(),
            m_PresentQueue.Index.value()
        };

        std::vector<VkDeviceQueueCreateInfo> queueInfos;
        for (u32 index : queueIndices) {
            VkDeviceQueueCreateInfo info;
            info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = 0;
            info.queueFamilyIndex = index;
            info.queueCount = 1;
            info.pQueuePriorities = queuePriority;

            queueInfos.push_back(info);
        }

        VkPhysicalDeviceFeatures features;
        features.robustBufferAccess = VK_FALSE;
        features.fullDrawIndexUint32 = VK_FALSE;
        features.imageCubeArray = VK_FALSE;
        features.independentBlend = VK_FALSE;
        features.geometryShader = VK_FALSE;
        features.tessellationShader = VK_FALSE;
        features.sampleRateShading = VK_FALSE;
        features.dualSrcBlend = VK_FALSE;
        features.logicOp = VK_FALSE;
        features.multiDrawIndirect = VK_FALSE;
        features.drawIndirectFirstInstance = VK_FALSE;
        features.depthClamp = VK_FALSE;
        features.depthBiasClamp = VK_FALSE;
        features.fillModeNonSolid = VK_FALSE;
        features.depthBounds = VK_FALSE;
        features.wideLines = VK_FALSE;
        features.largePoints = VK_FALSE;
        features.alphaToOne = VK_FALSE;
        features.multiViewport = VK_FALSE;
        features.samplerAnisotropy = VK_FALSE;
        features.textureCompressionETC2 = VK_FALSE;
        features.textureCompressionASTC_LDR = VK_FALSE;
        features.textureCompressionBC = VK_FALSE;
        features.occlusionQueryPrecise = VK_FALSE;
        features.pipelineStatisticsQuery = VK_FALSE;
        features.vertexPipelineStoresAndAtomics = VK_FALSE;
        features.fragmentStoresAndAtomics = VK_FALSE;
        features.shaderTessellationAndGeometryPointSize = VK_FALSE;
        features.shaderImageGatherExtended = VK_FALSE;
        features.shaderStorageImageExtendedFormats = VK_FALSE;
        features.shaderStorageImageMultisample = VK_FALSE;
        features.shaderStorageImageReadWithoutFormat = VK_FALSE;
        features.shaderStorageImageWriteWithoutFormat = VK_FALSE;
        features.shaderUniformBufferArrayDynamicIndexing = VK_FALSE;
        features.shaderSampledImageArrayDynamicIndexing = VK_FALSE;
        features.shaderStorageBufferArrayDynamicIndexing = VK_FALSE;
        features.shaderStorageImageArrayDynamicIndexing = VK_FALSE;
        features.shaderClipDistance = VK_FALSE;
        features.shaderCullDistance = VK_FALSE;
        features.shaderFloat64 = VK_FALSE;
        features.shaderInt64 = VK_FALSE;
        features.shaderInt16 = VK_FALSE;
        features.shaderResourceResidency = VK_FALSE;
        features.shaderResourceMinLod = VK_FALSE;
        features.sparseBinding = VK_FALSE;
        features.sparseResidencyBuffer = VK_FALSE;
        features.sparseResidencyImage2D = VK_FALSE;
        features.sparseResidencyImage3D = VK_FALSE;
        features.sparseResidency2Samples = VK_FALSE;
        features.sparseResidency4Samples = VK_FALSE;
        features.sparseResidency8Samples = VK_FALSE;
        features.sparseResidency16Samples = VK_FALSE;
        features.sparseResidencyAliased = VK_FALSE;
        features.variableMultisampleRate = VK_FALSE;
        features.inheritedQueries = VK_FALSE;

        std::vector<const char*> extensions;
        extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        VkDeviceCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.queueCreateInfoCount = queueInfos.size();
        createInfo.pQueueCreateInfos = queueInfos.data();
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.pEnabledFeatures = &features;

        if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create device");
        }

        vkGetDeviceQueue(m_Device, m_GraphicsQueue.Index.value(), 0, &m_GraphicsQueue.Queue);
        vkGetDeviceQueue(m_Device, m_PresentQueue.Index.value(), 0, &m_PresentQueue.Queue);
    }

    void Renderer::QuerySurfaceCapabilities()
    {
        u32 formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &formatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &formatCount, surfaceFormats.data());

        bool preferredFormat = false;
        for (const auto& format : surfaceFormats) {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                m_SurfaceFormat = format;
                preferredFormat = true;
                break;
            }
        }

        if (!preferredFormat) {
            m_SurfaceFormat = surfaceFormats[0];
        }

        u32 presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, presentModes.data());

        bool preferredPresentMode = false;
        for (const auto& presentMode : presentModes) {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                m_PresentMode = presentMode;
                preferredPresentMode = true;
            }
        }

        if (!preferredPresentMode) {
            m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;
        }

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &m_SurfaceCapabilities);
    }

    void Renderer::SetExtent(u32 width, u32 height)
    {
        if (m_SurfaceCapabilities.currentExtent.width != std::numeric_limits<u32>::max()) {
            m_Extent = m_SurfaceCapabilities.currentExtent;
        } else {
            width = std::clamp(width, m_SurfaceCapabilities.minImageExtent.width, m_SurfaceCapabilities.maxImageExtent.width);
            height = std::clamp(height, m_SurfaceCapabilities.minImageExtent.height, m_SurfaceCapabilities.maxImageExtent.height);

            m_Extent = { width, height };
        }
    }

    void Renderer::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment;
        colorAttachment.flags = 0;
        colorAttachment.format = m_SurfaceFormat.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef;
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass;
        subpass.flags = 0;
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.inputAttachmentCount = 0;
        subpass.pInputAttachments = nullptr;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pResolveAttachments = nullptr;
        subpass.pDepthStencilAttachment = nullptr;
        subpass.preserveAttachmentCount = 0;
        subpass.pPreserveAttachments = nullptr;

        VkSubpassDependency dependency;
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependency.dependencyFlags = 0;

        VkRenderPassCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &colorAttachment;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subpass;
        createInfo.dependencyCount = 1;
        createInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(m_Device, &createInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create render pass");
        }
    }

    void Renderer::CreateSwapchain()
    {
        u32 imageCount = m_SurfaceCapabilities.minImageCount + 1;
        if (m_SurfaceCapabilities.maxImageCount > 0 && imageCount > m_SurfaceCapabilities.maxImageCount) {
            imageCount = m_SurfaceCapabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.surface = m_Surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = m_SurfaceFormat.format;
        createInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
        createInfo.imageExtent = m_Extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        if (m_GraphicsQueue.Index.value() != m_PresentQueue.Index.value()) {
            u32 queueIndices[] = { m_GraphicsQueue.Index.value(), m_PresentQueue.Index.value() };
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = sizeof(queueIndices) / sizeof(queueIndices[0]);
            createInfo.pQueueFamilyIndices = queueIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = m_SurfaceCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = m_PresentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create");
        }

        vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &imageCount, nullptr);
        m_Images.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &imageCount, m_Images.data());

        m_ImageViews.resize(imageCount);
        for (u32 i = 0; i < imageCount; ++i) {
            VkImageViewCreateInfo imageViewInfo;
            imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewInfo.pNext = nullptr;
            imageViewInfo.flags = 0;
            imageViewInfo.image = m_Images[i];
            imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewInfo.format = m_SurfaceFormat.format;
            imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewInfo.subresourceRange.baseMipLevel = 0;
            imageViewInfo.subresourceRange.levelCount = 1;
            imageViewInfo.subresourceRange.baseArrayLayer = 0;
            imageViewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_Device, &imageViewInfo, nullptr, &m_ImageViews[i]) != VK_SUCCESS) {
                M_CORE_ERROR("Failed to create image view, index {}", i);
            }
        }

        m_Framebuffers.resize(imageCount);
        for (u32 i = 0; i < imageCount; ++i) {
            VkFramebufferCreateInfo framebufferInfo;
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.pNext = nullptr;
            framebufferInfo.flags = 0;
            framebufferInfo.renderPass = m_RenderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = &m_ImageViews[i];
            framebufferInfo.width = m_Extent.width;
            framebufferInfo.height = m_Extent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS) {
                M_CORE_ERROR("Failed to create framebuffer, index {}", i);
            }
        }
    }

    VkShaderModule Renderer::CompileShaderFile(const std::string& filepath)
    {
        std::ifstream file(filepath, std::ios::binary | std::ios::ate);

        if (!file.is_open()) {
            M_CORE_ERROR("Failed to open shader file {}", filepath);
        }

        u64 fileSize = (u64)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        VkShaderModuleCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.codeSize = fileSize;
        createInfo.pCode = reinterpret_cast<const u32*>(buffer.data());

        VkShaderModule module;
        if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &module) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create shader module for file {}", filepath);
        }

        return module;
    }

    void Renderer::CreateGraphicsPipeline()
    {
        VkShaderModule vertexModule = CompileShaderFile("Shaders/Triangle.vert.spv");
        VkShaderModule fragmentModule = CompileShaderFile("Shaders/Triangle.frag.spv");

        VkPipelineShaderStageCreateInfo shaderStages[2];
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].flags = 0;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vertexModule;
        shaderStages[0].pName = "main";
        shaderStages[0].pSpecializationInfo = nullptr;

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].flags = 0;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = fragmentModule;
        shaderStages[1].pName = "main";
        shaderStages[1].pSpecializationInfo = nullptr;

        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.pNext = nullptr;
        dynamicStateInfo.flags = 0;
        dynamicStateInfo.dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
        dynamicStateInfo.pDynamicStates = dynamicStates;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo;
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.pNext = nullptr;
        vertexInputInfo.flags = 0;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.pNext = nullptr;
        inputAssemblyInfo.flags = 0;
        inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = m_Extent.width;
        viewport.height = m_Extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor;
        scissor.offset = {0, 0};
        scissor.extent = m_Extent;

        VkPipelineViewportStateCreateInfo viewportInfo;
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.pNext = nullptr;
        viewportInfo.flags = 0;
        viewportInfo.viewportCount = 1;
        viewportInfo.pViewports = &viewport;
        viewportInfo.scissorCount = 1;
        viewportInfo.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationInfo.pNext = nullptr;
        rasterizationInfo.flags = 0;
        rasterizationInfo.depthClampEnable = VK_FALSE;
        rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizationInfo.depthBiasEnable = VK_FALSE;
        rasterizationInfo.depthBiasConstantFactor = 0.0f;
        rasterizationInfo.depthBiasClamp = 0.0f;
        rasterizationInfo.depthBiasSlopeFactor = 0.0f;
        rasterizationInfo.lineWidth = 1.0f;

        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleInfo.pNext = nullptr;
        multisampleInfo.flags = 0;
        multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleInfo.sampleShadingEnable = VK_FALSE;
        multisampleInfo.minSampleShading = 1.0f;
        multisampleInfo.pSampleMask = nullptr;
        multisampleInfo.alphaToCoverageEnable = VK_FALSE;
        multisampleInfo.alphaToOneEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.pNext = nullptr;
        colorBlendInfo.flags = 0;
        colorBlendInfo.logicOpEnable = VK_FALSE;
        colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
        colorBlendInfo.attachmentCount = 1;
        colorBlendInfo.pAttachments = &colorBlendAttachment;
        colorBlendInfo.blendConstants[0] = 0.0f;
        colorBlendInfo.blendConstants[1] = 0.0f;
        colorBlendInfo.blendConstants[2] = 0.0f;
        colorBlendInfo.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo layoutInfo;
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.pNext = nullptr;
        layoutInfo.flags = 0;
        layoutInfo.setLayoutCount = 0;
        layoutInfo.pSetLayouts = nullptr;
        layoutInfo.pushConstantRangeCount = 0;
        layoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(m_Device, &layoutInfo, nullptr, &m_GraphicsPipelineLayout) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create graphics pipeline layout");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo;
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = nullptr;
        pipelineInfo.flags = 0;
        pipelineInfo.stageCount = sizeof(shaderStages) / sizeof(shaderStages[0]);
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
        pipelineInfo.pTessellationState = nullptr;
        pipelineInfo.pViewportState = &viewportInfo;
        pipelineInfo.pRasterizationState = &rasterizationInfo;
        pipelineInfo.pMultisampleState = &multisampleInfo;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.pColorBlendState = &colorBlendInfo;
        pipelineInfo.pDynamicState = &dynamicStateInfo;
        pipelineInfo.layout = m_GraphicsPipelineLayout;
        pipelineInfo.renderPass = m_RenderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS) {
            fprintf(stderr, "failed to create graphics pipeline\n");
        }

        vkDestroyShaderModule(m_Device, fragmentModule, nullptr);
        vkDestroyShaderModule(m_Device, vertexModule, nullptr);
    }

    void Renderer::CreateCommandPool()
    {
        VkCommandPoolCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        createInfo.queueFamilyIndex = m_GraphicsQueue.Index.value();

        if (vkCreateCommandPool(m_Device, &createInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create command pool");
        }
    }

    void Renderer::AllocateCommandBuffer()
    {
        m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocateInfo;
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.pNext = nullptr;
        allocateInfo.commandPool = m_CommandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

        if (vkAllocateCommandBuffers(m_Device, &allocateInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to allocated command buffers");
        }
    }

    void Renderer::CreateSyncObjects()
    {
        m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo;
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreInfo.pNext = nullptr;
        semaphoreInfo.flags = 0;

        VkFenceCreateInfo fenceInfo;
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = nullptr;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS) {
                M_CORE_ERROR("Failed to create image available semaphore, index {}", i);
            }
            if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS) {
                M_CORE_ERROR("Failed to create render finished semaphore, index {}", i);
            }
            if (vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS) {
                M_CORE_ERROR("Failed to create in flight fence, index {}", i);
            }
        }
    }

    void Renderer::RecordCommandBuffer(VkCommandBuffer commandBuffer, u32 index)
    {
        VkCommandBufferBeginInfo beginInfo;
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = NULL;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = NULL;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to begin recording command buffer");
        }

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

        VkRenderPassBeginInfo renderPassInfo;
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.pNext = NULL;
        renderPassInfo.renderPass = m_RenderPass;
        renderPassInfo.framebuffer = m_Framebuffers[index];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_Extent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = m_Extent.width;
        viewport.height = m_Extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor;
        scissor.offset = {0, 0};
        scissor.extent = m_Extent;

        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            fprintf(stderr, "failed to record command buffer\n");
        }
    }

    void Renderer::RecreateSwapchain()
    {
        vkDeviceWaitIdle(m_Device);

        vkDestroyPipeline(m_Device, m_GraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(m_Device, m_GraphicsPipelineLayout, nullptr);
        for (auto& framebuffer : m_Framebuffers) {
            vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
        }
        for (auto& imageView : m_ImageViews) {
            vkDestroyImageView(m_Device, imageView, nullptr);
        }
        vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
        vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);

        CreateRenderPass();
        CreateSwapchain();
        CreateGraphicsPipeline();
    }

}
