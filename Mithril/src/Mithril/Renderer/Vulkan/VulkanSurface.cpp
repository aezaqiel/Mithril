#include "VulkanSurface.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Mithril/Core/Application.hpp"
#include "Mithril/Core/Logger.hpp"

namespace Mithril {

    VulkanSurface::VulkanSurface(const Ref<VulkanContext>& context)
        : m_Context(context)
    {
        if (glfwCreateWindowSurface(m_Context->Instance(), static_cast<GLFWwindow*>(Application::Instance().GetWindow()->Native()), nullptr, &m_Surface) != VK_SUCCESS) {
            M_CORE_ERROR("Failed to create surface");
        }
    }

    VulkanSurface::~VulkanSurface()
    {
        vkDestroySurfaceKHR(m_Context->Instance(), m_Surface, nullptr);
    }

}
