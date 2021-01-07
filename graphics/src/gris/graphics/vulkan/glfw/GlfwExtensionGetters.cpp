#include <gris/graphics/vulkan/glfw/GlfwExtensionGetters.h>

#include <GLFW/glfw3.h>

// -------------------------------------------------------------------------------------------------

std::vector<const char*> Gris::Graphics::Vulkan::Glfw::GetVulkanInstanceExtensionsFromGLFW()
{
    uint32_t glfwExtensionCount = 0;
    auto* const glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}