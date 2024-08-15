#include "Renderer.hpp"

#include "Mithril/Core/Logger.hpp"

namespace Mithril {

    Renderer::Renderer()
    {
        if (volkInitialize() != VK_SUCCESS) {
            M_CORE_ERROR("Failed to initialize vulkan loader");
        }

        VkApplicationInfo appInfo;
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;
        appInfo.pApplicationName = "Application";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Mithril";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = m_Layers.size();
        createInfo.ppEnabledLayerNames = m_Layers.data();
        createInfo.enabledExtensionCount = m_Extensions.size();
        createInfo.ppEnabledExtensionNames = m_Extensions.data();

        if (vkCreateInstance(&createInfo, m_Allocator, &m_Instance) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create vulkan instance");
        }

        volkLoadInstance(m_Instance);
    }

    Renderer::~Renderer()
    {
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

}
