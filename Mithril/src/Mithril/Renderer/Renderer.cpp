#include "Renderer.hpp"

#include <sstream>
#include <set>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Mithril/Core/Application.hpp"
#include "Mithril/Core/Logger.hpp"

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
            M_CORE_ERROR("Failed to initialize vulkan loader");
        }
        
        CreateInstance();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
    }

    Renderer::~Renderer()
    {
        vkDestroyDevice(m_Device, m_Allocator);
        vkDestroySurfaceKHR(m_Instance, m_Surface, m_Allocator);
#ifndef NDEBUG
        vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, m_Allocator);
#endif
        vkDestroyInstance(m_Instance, m_Allocator);
    }

    void Renderer::Draw()
    {
    }

    void Renderer::Resize(uint32_t width, uint32_t height)
    {
        (void)width;
        (void)height;
    }

    void Renderer::CreateInstance()
    {
        VkApplicationInfo appInfo;
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;
        appInfo.pApplicationName = "Application";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Mithril";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        for (uint32_t i = 0; i < glfwExtensionCount; ++i) {
            m_Extensions.push_back(glfwExtensions[i]);
        }

#ifndef NDEBUG
        m_Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        m_Layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

        VkInstanceCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = m_Layers.size();
        createInfo.ppEnabledLayerNames = m_Layers.data();
        createInfo.enabledExtensionCount = m_Extensions.size();
        createInfo.ppEnabledExtensionNames = m_Extensions.data();

#ifndef NDEBUG
        VkDebugUtilsMessengerCreateInfoEXT messengerInfo;
        messengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        messengerInfo.pNext = nullptr;
        messengerInfo.flags = 0;
        messengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        messengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
        messengerInfo.pfnUserCallback = DebugCallback;
        messengerInfo.pUserData = nullptr;

        createInfo.pNext = &messengerInfo;
#endif

        if (vkCreateInstance(&createInfo, m_Allocator, &m_Instance) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create vulkan instance");
        }

        volkLoadInstance(m_Instance);

#ifndef NDEBUG
        if (vkCreateDebugUtilsMessengerEXT(m_Instance, &messengerInfo, m_Allocator, &m_DebugMessenger) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create debug messenger");
        }
#endif
    }

    void Renderer::CreateSurface()
    {
        if (glfwCreateWindowSurface(m_Instance, static_cast<GLFWwindow*>(Application::Instance().GetWindow()->Native()), m_Allocator, &m_Surface) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create surface");
        }
    }

    void Renderer::PickPhysicalDevice()
    {
        m_PhysicalDevice = VK_NULL_HANDLE;

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            M_CORE_ERROR("Failed to find GPUs with vulkan support");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (DeviceSuitable(device)) {
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
    }

    bool Renderer::DeviceSuitable(VkPhysicalDevice device)
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

        QueueFamilyIndices indices = FindQueueFamilies(device);

        if (!indices.GraphicsFamily.has_value()) {
            M_CORE_TRACE("Does not support graphics");
            return false;
        }

        if (!indices.PresentFamily.has_value()) {
            M_CORE_TRACE("Does not support surface");
            return false;
        }

        return true;
    }

    Renderer::QueueFamilyIndices Renderer::FindQueueFamilies(VkPhysicalDevice device)
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
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

            if (presentSupport == VK_TRUE) {
                indices.PresentFamily = i;
            }

            i++;
        }

        return indices;
    }

    void Renderer::CreateLogicalDevice()
    {
        QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

        std::set<uint32_t> queueFamilies = {indices.GraphicsFamily.value(), indices.PresentFamily.value()};

        float priority[] = { 1.0f };

        std::vector<VkDeviceQueueCreateInfo> queueInfos;
        for (uint32_t family : queueFamilies) {
            VkDeviceQueueCreateInfo queueInfo;
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.pNext = nullptr;
            queueInfo.flags = 0;
            queueInfo.queueFamilyIndex = family;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = priority;

            queueInfos.push_back(queueInfo);
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

        VkDeviceCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.queueCreateInfoCount = queueInfos.size();
        createInfo.pQueueCreateInfos = queueInfos.data();
        createInfo.enabledLayerCount = m_Layers.size();
        createInfo.ppEnabledLayerNames = m_Layers.data();
        createInfo.enabledExtensionCount = 0;
        createInfo.ppEnabledExtensionNames = nullptr;
        createInfo.pEnabledFeatures = &features;

        if (vkCreateDevice(m_PhysicalDevice, &createInfo, m_Allocator, &m_Device) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create logical device");
        }

        volkLoadDevice(m_Device);

        vkGetDeviceQueue(m_Device, indices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, indices.PresentFamily.value(), 0, &m_PresentQueue);
    }

}
