#pragma once

#include <vector>

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
        void CreateInstance();

    private:
        VkAllocationCallbacks* m_Allocator { nullptr };

        std::vector<const char*> m_Layers;
        std::vector<const char*> m_Extensions;

        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
    };

}
