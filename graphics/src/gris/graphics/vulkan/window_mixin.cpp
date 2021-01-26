#include <gris/graphics/vulkan/window_mixin.h>

#include <gris/graphics/vulkan/instance.h>

// -------------------------------------------------------------------------------------------------

vk::Instance & Gris::Graphics::Vulkan::WindowMixin::InstanceHandle()
{
    return Instance::Get().InstanceHandle();
}

// -------------------------------------------------------------------------------------------------

vk::DispatchLoaderDynamic & Gris::Graphics::Vulkan::WindowMixin::Dispatch()
{
    return Instance::Get().Dispatch();
}
