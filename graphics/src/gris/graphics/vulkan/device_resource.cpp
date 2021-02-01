#include <gris/graphics/vulkan/device_resource.h>

#include <gris/graphics/vulkan/device.h>

#include <gris/assert.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::DeviceResource::DeviceResource(Device * device)
    : m_parentDevice(device)
{
    GRIS_ALWAYS_ASSERT(m_parentDevice != nullptr, "Parent device cannot be null");
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

[[nodiscard]] const vk::DispatchLoaderDynamic & Gris::Graphics::Vulkan::DeviceResource::Dispatch() const
{
    return m_parentDevice->m_dispatch;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::DispatchLoaderDynamic & Gris::Graphics::Vulkan::DeviceResource::Dispatch()
{
    return m_parentDevice->m_dispatch;
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

[[nodiscard]] const Gris::Graphics::Vulkan::ShaderResourceBindingsPoolManager & Gris::Graphics::Vulkan::DeviceResource::PoolManager(Backend::ShaderResourceBindingsPoolCategory category) const
{
    return m_parentDevice->PoolManager(category);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::ShaderResourceBindingsPoolManager & Gris::Graphics::Vulkan::DeviceResource::PoolManager(Backend::ShaderResourceBindingsPoolCategory category)
{
    return m_parentDevice->PoolManager(category);
}
