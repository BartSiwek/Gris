#include <gris/graphics/vulkan/glfw/GlfwVulkanWindow.h>

#include <gris/graphics/vulkan/VulkanInstance.h>
#include <gris/graphics/vulkan/VulkanEngineException.h>

#include <GLFW/glfw3.h>

// -------------------------------------------------------------------------------------------------

GlfwVulkanWindow::GlfwVulkanWindow(uint32_t width, uint32_t height, const std::string& title) : GlfwWindowMixin(width, height, title), VulkanWindowMixin()
{
    VkSurfaceKHR surface;
    auto const createSurfaceResult = static_cast<vk::Result>(glfwCreateWindowSurface(VulkanInstanceHandle(), WindowHandle(), nullptr, &surface));
    if (createSurfaceResult != vk::Result::eSuccess)
        throw VulkanEngineException("Failed to create GLFW window surface", createSurfaceResult);

    const vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> deleter(VulkanInstanceHandle());
    SetSurfaceHandle(vk::UniqueSurfaceKHR(vk::SurfaceKHR(surface), deleter));
}
