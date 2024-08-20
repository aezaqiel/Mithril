#pragma once
#include "Mithril/Defines.hpp"

#include "VulkanContext.hpp"

namespace Mithril {

    class VulkanSurface
    {
    public:
        VulkanSurface(const Ref<VulkanContext>& context);
        ~VulkanSurface();

        inline VkSurfaceKHR Surface() const { return m_Surface; }
    private:
        Ref<VulkanContext> m_Context;
        VkSurfaceKHR m_Surface;
    };

}
