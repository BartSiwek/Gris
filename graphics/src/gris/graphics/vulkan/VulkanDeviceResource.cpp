#include <gris/graphics/vulkan/VulkanDeviceResource.h>

#include <gris/graphics/vulkan/VulkanDevice.h>

#include <gris/assert.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanDeviceResource::VulkanDeviceResource(VulkanDevice* device) : m_parentDevice(device)
{
    GRIS_ALAWYS_ASSERT(m_parentDevice != nullptr, "Parent device cannot be null");
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::VulkanDevice& Gris::Graphics::Vulkan::VulkanDeviceResource::ParentDevice() const
{
    return *m_parentDevice;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanDevice& Gris::Graphics::Vulkan::VulkanDeviceResource::ParentDevice()
{
    return *m_parentDevice;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::Device& Gris::Graphics::Vulkan::VulkanDeviceResource::DeviceHandle() const
{
    return m_parentDevice->DeviceHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Device& Gris::Graphics::Vulkan::VulkanDeviceResource::DeviceHandle()
{
    return m_parentDevice->DeviceHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::VulkanAllocator& Gris::Graphics::Vulkan::VulkanDeviceResource::Allocator() const
{
    return m_parentDevice->Allocator();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanAllocator& Gris::Graphics::Vulkan::VulkanDeviceResource::Allocator()
{
    return m_parentDevice->Allocator();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::DescriptorPool& Gris::Graphics::Vulkan::VulkanDeviceResource::DescriptorPoolHandle() const
{
    return m_parentDevice->DescriptorPoolHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::DescriptorPool& Gris::Graphics::Vulkan::VulkanDeviceResource::DescriptorPoolHandle()
{
    return m_parentDevice->DescriptorPoolHandle();
}
