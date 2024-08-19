#pragma once

#include <vector>
#include <optional>

#include <volk.h>

namespace Mithril {

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void Draw();

        void Resize(uint32_t width, uint32_t height);
    
    private:
        struct QueueFamilyIndices
        {
            std::optional<uint32_t> GraphicsFamily;
            std::optional<uint32_t> PresentFamily;
        };


    private:
        void CreateInstance();
        void CreateSurface();
        void PickPhysicalDevice();
        bool DeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
        void CreateLogicalDevice();

    private:
        VkAllocationCallbacks* m_Allocator { nullptr };

        std::vector<const char*> m_Layers;
        std::vector<const char*> m_Extensions;

        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkSurfaceKHR m_Surface;
        VkPhysicalDevice m_PhysicalDevice;
        QueueFamilyIndices m_Indices;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;
        VkDevice m_Device;
    };

}
