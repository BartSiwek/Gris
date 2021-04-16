#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

namespace Gris::Graphics::Vulkan
{

class WindowMixin
{
public:
    WindowMixin() = default;

    WindowMixin(const WindowMixin & other) = delete;
    WindowMixin & operator=(const WindowMixin & other) = delete;

    WindowMixin(WindowMixin && other) noexcept;
    WindowMixin & operator=(WindowMixin && other) noexcept;

    virtual ~WindowMixin();

    [[nodiscard]] bool IsWindowValid() const;

    [[nodiscard]] vk::SurfaceKHR SurfaceHandle() const
    {
        return m_surface;
    }

protected:
    static vk::Instance & InstanceHandle();

    static vk::DispatchLoaderDynamic & Dispatch();

    void SetSurfaceHandle(vk::SurfaceKHR surface)
    {
        m_surface = surface;
    }

private:
    void Reset();

    vk::SurfaceKHR m_surface = {};
};

}  // namespace Gris::Graphics::Vulkan
