#pragma once
#include "Mithril/Defines.hpp"

#include <vector>

#include <volk.h>

namespace Mithril {

    class VulkanContext
    {
    public:
        VulkanContext();
        ~VulkanContext();

        inline VkInstance Instance() const { return m_Instance; }
    
    private:
        void CheckLayers();
        void CheckExtensions();

    private:
        std::vector<const char*> m_Layers;
        std::vector<const char*> m_Extensions;
        VkInstance m_Instance { VK_NULL_HANDLE };
        VkDebugUtilsMessengerEXT m_DebugMessenger { VK_NULL_HANDLE };
    };

}
