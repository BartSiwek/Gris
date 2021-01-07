#pragma once

#define VULKAN_HPP_TYPESAFE_CONVERSION
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

#include <gris/assert.h>

namespace Gris::Graphics::Vulkan
{

class VulkanWindowMixin
{
public:
    VulkanWindowMixin() = default;
    virtual ~VulkanWindowMixin() = default;

    VulkanWindowMixin(const VulkanWindowMixin& other) = delete;
    VulkanWindowMixin& operator=(const VulkanWindowMixin& other) = delete;

    VulkanWindowMixin(VulkanWindowMixin&&) noexcept = default;
    VulkanWindowMixin& operator=(VulkanWindowMixin&&) noexcept = default;

    [[nodiscard]] vk::SurfaceKHR SurfaceHandle() const
    {
        GRIS_ALAWYS_ASSERT(m_surface, "Invalid surface handle");
        return m_surface.get();
    }

protected:
    static vk::Instance VulkanInstanceHandle();

    void SetSurfaceHandle(vk::UniqueSurfaceKHR surface)
    {
        m_surface = std::move(surface);
    }

private:
    vk::UniqueSurfaceKHR m_surface = {};
};

}  // namespace Gris::Graphics::Vulkan
