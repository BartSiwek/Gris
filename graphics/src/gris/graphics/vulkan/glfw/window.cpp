#include <gris/graphics/vulkan/glfw/window.h>

#include <gris/graphics/vulkan/instance.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

#include <GLFW/glfw3.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Glfw::Window::Window() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Glfw::Window::Window(uint32_t width, uint32_t height, const std::string & title)
    : Gris::Graphics::Glfw::WindowMixin(width, height, title)
{
    VkSurfaceKHR surface = nullptr;
    auto const createSurfaceResult = static_cast<vk::Result>(glfwCreateWindowSurface(InstanceHandle(), WindowHandle(), nullptr, &surface));
    if (createSurfaceResult != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Failed to create GLFW window surface", createSurfaceResult);
    }

    SetSurfaceHandle(vk::SurfaceKHR(surface));
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Glfw::Window::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::Glfw::Window::IsValid() const
{
    return Graphics::Glfw::WindowMixin::IsWindowValid() && Graphics::Vulkan::WindowMixin::IsWindowValid();
}
