#include <gris/graphics/vulkan/device_resource.h>

#include <gris/graphics/vulkan/device.h>

#include <gris/assert.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::DeviceResource::DeviceResource() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::DeviceResource::DeviceResource(std::shared_ptr<Device *> device)
    : m_parentDevice(std::move(device))
{
    GRIS_ALWAYS_ASSERT(m_parentDevice != nullptr, "Parent device cannot be null");
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::DeviceResource::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

bool Gris::Graphics::Vulkan::DeviceResource::IsValid() const
{
    return static_cast<bool>(m_parentDevice) && *m_parentDevice != nullptr;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::Device & Gris::Graphics::Vulkan::DeviceResource::ParentDevice() const
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_parentDevice), "A parent pointer pointer is null");
    GRIS_ALWAYS_ASSERT(*m_parentDevice != nullptr, "A parent pointer is null");
    return **m_parentDevice;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Device & Gris::Graphics::Vulkan::DeviceResource::ParentDevice()
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_parentDevice), "A parent pointer pointer is null");
    GRIS_ALWAYS_ASSERT(*m_parentDevice != nullptr, "A parent pointer is null");
    return **m_parentDevice;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::Device & Gris::Graphics::Vulkan::DeviceResource::DeviceHandle() const
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_parentDevice), "A parent pointer pointer is null");
    GRIS_ALWAYS_ASSERT(*m_parentDevice != nullptr, "A parent pointer is null");
    return (*m_parentDevice)->DeviceHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Device & Gris::Graphics::Vulkan::DeviceResource::DeviceHandle()
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_parentDevice), "A parent pointer pointer is null");
    GRIS_ALWAYS_ASSERT(*m_parentDevice != nullptr, "A parent pointer is null");
    return (*m_parentDevice)->DeviceHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::DispatchLoaderDynamic & Gris::Graphics::Vulkan::DeviceResource::Dispatch() const
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_parentDevice), "A parent pointer pointer is null");
    GRIS_ALWAYS_ASSERT(*m_parentDevice != nullptr, "A parent pointer is null");
    return (*m_parentDevice)->m_dispatch;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::DispatchLoaderDynamic & Gris::Graphics::Vulkan::DeviceResource::Dispatch()
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_parentDevice), "A parent pointer pointer is null");
    GRIS_ALWAYS_ASSERT(*m_parentDevice != nullptr, "A parent pointer is null");
    return (*m_parentDevice)->m_dispatch;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::Allocator & Gris::Graphics::Vulkan::DeviceResource::AllocatorHandle() const
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_parentDevice), "A parent pointer pointer is null");
    GRIS_ALWAYS_ASSERT(*m_parentDevice != nullptr, "A parent pointer is null");
    return (*m_parentDevice)->AllocatorHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Allocator & Gris::Graphics::Vulkan::DeviceResource::AllocatorHandle()
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_parentDevice), "A parent pointer pointer is null");
    GRIS_ALWAYS_ASSERT(*m_parentDevice != nullptr, "A parent pointer is null");
    return (*m_parentDevice)->AllocatorHandle();
}
