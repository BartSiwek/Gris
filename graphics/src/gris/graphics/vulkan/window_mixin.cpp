#include <gris/graphics/vulkan/window_mixin.h>

#include <gris/graphics/vulkan/instance.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::WindowMixin::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

bool Gris::Graphics::Vulkan::WindowMixin::IsValid() const
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
