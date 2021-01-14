#include <gris/graphics/vulkan/device_resource.h>

#include <gris/graphics/vulkan/device.h>

#include <gris/assert.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::DeviceResource::DeviceResource(Device * device)
    : m_parentDevice(device)
{
    GRIS_ALAWYS_ASSERT(m_parentDevice != nullptr, "Parent device cannot be null");
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::Device & Gris::Graphics::Vulkan::DeviceResource::ParentDevice() const
{
    return *m_parentDevice;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Device & Gris::Graphics::Vulkan::DeviceResource::ParentDevice()
{
    return *m_parentDevice;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::Device & Gris::Graphics::Vulkan::DeviceResource::DeviceHandle() const
{
    return m_parentDevice->DeviceHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Device & Gris::Graphics::Vulkan::DeviceResource::DeviceHandle()
{
    return m_parentDevice->DeviceHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::Allocator & Gris::Graphics::Vulkan::DeviceResource::AllocatorHandle() const
{
    return m_parentDevice->AllocatorHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Allocator & Gris::Graphics::Vulkan::DeviceResource::AllocatorHandle()
{
    return m_parentDevice->AllocatorHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::DescriptorPool & Gris::Graphics::Vulkan::DeviceResource::DescriptorPoolHandle() const
{
    return m_parentDevice->DescriptorPoolHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::DescriptorPool & Gris::Graphics::Vulkan::DeviceResource::DescriptorPoolHandle()
{
    return m_parentDevice->DescriptorPoolHandle();
}
