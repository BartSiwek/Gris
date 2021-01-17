#include <gris/graphics/vulkan/glfw/extension_getters.h>

#include <GLFW/glfw3.h>

// -------------------------------------------------------------------------------------------------

std::vector<const char *> Gris::Graphics::Vulkan::Glfw::GetInstanceExtensionsFromGLFW()
{
    uint32_t glfwExtensionCount = 0;
    auto * const glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}