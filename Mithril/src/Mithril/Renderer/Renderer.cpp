#include "Renderer.hpp"

#include <sstream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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
        PickPhysicalDevice();
    }

    Renderer::~Renderer()
    {
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
            return false;
        }

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        if (!features.geometryShader) {
            return false;
        }

        return true;
    }

}
