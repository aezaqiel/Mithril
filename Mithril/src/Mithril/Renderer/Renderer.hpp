#pragma once
#include "Mithril/Defines.hpp"

#include "Vulkan/VulkanContext.hpp"
#include "Vulkan/VulkanSurface.hpp"
#include "Vulkan/VulkanDevice.hpp"

namespace Mithril {

    class Renderer
    {
    public:
        Renderer();
        ~Renderer() = default;

        void Draw();

        void Resize(u32 width, u32 height);

    private:
        Ref<VulkanContext> m_Context;
        Ref<VulkanSurface> m_Surface;
        Ref<VulkanDevice> m_Device;
    };

}
