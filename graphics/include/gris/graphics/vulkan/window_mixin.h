#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

#include <gris/assert.h>

namespace Gris::Graphics::Vulkan
{

class WindowMixin
{
public:
    WindowMixin() = default;
    virtual ~WindowMixin() = default;

    WindowMixin(const WindowMixin & other) = delete;
    WindowMixin & operator=(const WindowMixin & other) = delete;

    WindowMixin(WindowMixin &&) noexcept = default;
    WindowMixin & operator=(WindowMixin &&) noexcept = default;

    [[nodiscard]] vk::SurfaceKHR SurfaceHandle() const
    {
        GRIS_ALWAYS_ASSERT(m_surface, "Invalid surface handle");
        return m_surface.get();
    }

protected:
    static vk::Instance & InstanceHandle();

    static vk::DispatchLoaderDynamic & Dispatch();

    void SetSurfaceHandle(vk::UniqueSurfaceKHR surface)
    {
        m_surface = std::move(surface);
    }

private:
    vk::UniqueSurfaceKHR m_surface = {};
};

}  // namespace Gris::Graphics::Vulkan
