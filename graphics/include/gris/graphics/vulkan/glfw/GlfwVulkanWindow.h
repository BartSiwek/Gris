#pragma once

#include <gris/graphics/glfw/GlfwWindowMixin.h>
#include <gris/graphics/vulkan/VulkanWindowMixin.h>

namespace Gris::Graphics::Vulkan::Glfw
{

class GlfwVulkanWindow : public Graphics::Glfw::GlfwWindowMixin
    , public VulkanWindowMixin
{
public:
    GlfwVulkanWindow(uint32_t width, uint32_t height, const std::string & title);

    GlfwVulkanWindow(const GlfwVulkanWindow &) = delete;
    GlfwVulkanWindow & operator=(const GlfwVulkanWindow &) = delete;

    GlfwVulkanWindow(GlfwVulkanWindow &&) noexcept = default;
    GlfwVulkanWindow & operator=(GlfwVulkanWindow &&) noexcept = default;

    ~GlfwVulkanWindow() override = default;
};

}  // namespace Gris::Graphics::Vulkan::Glfw
