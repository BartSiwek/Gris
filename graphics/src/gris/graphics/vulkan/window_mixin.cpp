#include <gris/graphics/vulkan/window_mixin.h>

#include <gris/graphics/vulkan/instance.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::WindowMixin::WindowMixin(WindowMixin && other) noexcept
    : m_surface(std::exchange(other.m_surface, {}))
{

}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::WindowMixin & Gris::Graphics::Vulkan::WindowMixin::operator=(WindowMixin && other) noexcept
{
    if (this != &other)
    {
        Reset();

        m_surface = std::exchange(other.m_surface, {});
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::WindowMixin::~WindowMixin()
{
    Reset();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::WindowMixin::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::WindowMixin::IsValid() const
{
    return static_cast<bool>(m_surface);
}

// -------------------------------------------------------------------------------------------------

vk::Instance & Gris::Graphics::Vulkan::WindowMixin::InstanceHandle()
{
    return Instance::InstanceHandle();
}

// -------------------------------------------------------------------------------------------------

vk::DispatchLoaderDynamic & Gris::Graphics::Vulkan::WindowMixin::Dispatch()
{
    return Instance::Dispatch();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::WindowMixin::Reset()
{
    if (m_surface)
    {
        InstanceHandle().destroySurfaceKHR(m_surface, nullptr, Dispatch());
        m_surface = nullptr;    
    }
}
