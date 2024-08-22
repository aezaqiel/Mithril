#include "VulkanContext.hpp"

#include "Mithril/Core/Logger.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Mithril {

#ifndef NDEBUG
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
#endif

    VulkanContext::VulkanContext()
    {
        if (volkInitialize() != VK_SUCCESS) {
            M_CORE_ERROR("Failed to initialize vulkan loader");
        }

        VkApplicationInfo info;
        info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        info.pNext = nullptr;
        info.pApplicationName = "Application";
        info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        info.pEngineName = "Mithril";
        info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        info.apiVersion = VK_API_VERSION_1_3;

#ifndef NDEBUG
        m_Layers.push_back("VK_LAYER_KHRONOS_validation");
        m_Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        u32 glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        for (u32 i = 0; i < glfwExtensionCount; ++i) {
            m_Extensions.push_back(glfwExtensions[i]);
        }

        CheckLayers();
        CheckExtensions();

#ifndef NDEBUG
        VkDebugUtilsMessengerCreateInfoEXT messengerInfo;
        messengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        messengerInfo.pNext = nullptr;
        messengerInfo.flags = 0;
        messengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        messengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
        messengerInfo.pfnUserCallback = DebugCallback;
        messengerInfo.pUserData = nullptr;
#endif

        VkInstanceCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.pApplicationInfo = &info;
        createInfo.enabledLayerCount = m_Layers.size();
        createInfo.ppEnabledLayerNames = m_Layers.data();
        createInfo.enabledExtensionCount = m_Extensions.size();
        createInfo.ppEnabledExtensionNames = m_Extensions.data();

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

    VulkanContext::~VulkanContext()
    {
#ifndef NDEBUG
        vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
#endif
        vkDestroyInstance(m_Instance, nullptr);
    }

    void VulkanContext::CheckLayers()
    {
        u32 count = 0;
        vkEnumerateInstanceLayerProperties(&count, nullptr);
        std::vector<VkLayerProperties> layers(count);
        vkEnumerateInstanceLayerProperties(&count, layers.data());

        for (const auto& requested : m_Layers) {
            bool found = false;
            for (const auto& available : layers) {
                if (strcmp(available.layerName, requested) == 0) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                M_CORE_ERROR("Requested layer {} not available", requested);
            }
        }
    }

    void VulkanContext::CheckExtensions()
    {
        u32 count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> extensions(count);
        vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());

        for (const auto& requested : m_Extensions) {
            bool found = false;
            for (const auto& available : extensions) {
                if (strcmp(available.extensionName, requested) == 0) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                M_CORE_ERROR("Requested instance extension {} not available", requested);
            }
        }
    }

}
