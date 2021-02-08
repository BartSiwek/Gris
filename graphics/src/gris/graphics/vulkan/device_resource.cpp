#include <gris/graphics/vulkan/device_resource.h>

#include <gris/graphics/vulkan/device.h>

#include <gris/assert.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::DeviceResource::DeviceResource() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::DeviceResource::DeviceResource(std::shared_ptr<DeviceSharedData> sharedData)
    : m_sharedData(std::move(sharedData))
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_sharedData), "Parent device cannot be null");
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::DeviceResource::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

bool Gris::Graphics::Vulkan::DeviceResource::IsValid() const
{
    return static_cast<bool>(m_sharedData) && m_sharedData->ParentDevice != nullptr;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::Device & Gris::Graphics::Vulkan::DeviceResource::ParentDevice() const
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_sharedData), "A parent pointer pointer is null");
    GRIS_ALWAYS_ASSERT(m_sharedData->ParentDevice != nullptr, "A parent pointer is null");
    return *m_sharedData->ParentDevice;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Device & Gris::Graphics::Vulkan::DeviceResource::ParentDevice()
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_sharedData), "A parent pointer pointer is null");
    GRIS_ALWAYS_ASSERT(m_sharedData->ParentDevice != nullptr, "A parent pointer is null");
    return *m_sharedData->ParentDevice;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::Device & Gris::Graphics::Vulkan::DeviceResource::DeviceHandle() const
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_sharedData), "A parent pointer pointer is null");
    GRIS_ALWAYS_ASSERT(m_sharedData->ParentDevice != nullptr, "A parent pointer is null");
    return m_sharedData->ParentDevice->DeviceHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Device & Gris::Graphics::Vulkan::DeviceResource::DeviceHandle()
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_sharedData), "A parent pointer pointer is null");
    GRIS_ALWAYS_ASSERT(m_sharedData->ParentDevice != nullptr, "A parent pointer is null");
    return m_sharedData->ParentDevice->DeviceHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::DispatchLoaderDynamic & Gris::Graphics::Vulkan::DeviceResource::Dispatch() const
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_sharedData), "A parent pointer pointer is null");
    GRIS_ALWAYS_ASSERT(m_sharedData->ParentDevice != nullptr, "A parent pointer is null");
    return m_sharedData->Dispatch;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::DispatchLoaderDynamic & Gris::Graphics::Vulkan::DeviceResource::Dispatch()
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_sharedData), "A parent pointer pointer is null");
    GRIS_ALWAYS_ASSERT(m_sharedData->ParentDevice != nullptr, "A parent pointer is null");
    return m_sharedData->Dispatch;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::Allocator & Gris::Graphics::Vulkan::DeviceResource::AllocatorHandle() const
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_sharedData), "A parent pointer pointer is null");
    GRIS_ALWAYS_ASSERT(m_sharedData->ParentDevice != nullptr, "A parent pointer is null");
    return m_sharedData->ParentDevice->AllocatorHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Allocator & Gris::Graphics::Vulkan::DeviceResource::AllocatorHandle()
{
    GRIS_ALWAYS_ASSERT(static_cast<bool>(m_sharedData), "A parent pointer pointer is null");
    GRIS_ALWAYS_ASSERT(m_sharedData->ParentDevice != nullptr, "A parent pointer is null");
    return m_sharedData->ParentDevice->AllocatorHandle();
}
