#include <gris/graphics/vulkan/device_resource.h>

#include <gris/graphics/vulkan/device.h>

#include <gris/assert.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::DeviceResource::DeviceResource() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::DeviceResource::DeviceResource(const ParentObject<Device> & device)
    : ChildObject(device)
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::DeviceResource::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::DeviceResource::IsValid() const
{
    return ChildObject::IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::Device & Gris::Graphics::Vulkan::DeviceResource::ParentDevice() const
{
    return Parent();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Device & Gris::Graphics::Vulkan::DeviceResource::ParentDevice()
{
    return Parent();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::Device & Gris::Graphics::Vulkan::DeviceResource::DeviceHandle() const
{
    return Parent().DeviceHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Device & Gris::Graphics::Vulkan::DeviceResource::DeviceHandle()
{
    return Parent().DeviceHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::DispatchLoaderDynamic & Gris::Graphics::Vulkan::DeviceResource::Dispatch() const
{
    return Parent().DispatchHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::DispatchLoaderDynamic & Gris::Graphics::Vulkan::DeviceResource::Dispatch()
{
    return Parent().DispatchHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::Allocator & Gris::Graphics::Vulkan::DeviceResource::AllocatorHandle() const
{
    return Parent().AllocatorHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Allocator & Gris::Graphics::Vulkan::DeviceResource::AllocatorHandle()
{
    return Parent().AllocatorHandle();
}
