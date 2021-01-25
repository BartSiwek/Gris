#include "extension_getters.h"

#include <GLFW/glfw3.h>

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Glfw::GetInstanceExtensionsFromGLFW(std::vector<const char *> * extensions)
{
    uint32_t glfwExtensionCount = 0;
    auto * const glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    extensions->insert(extensions->end(), glfwExtensions, glfwExtensions + glfwExtensionCount);
}