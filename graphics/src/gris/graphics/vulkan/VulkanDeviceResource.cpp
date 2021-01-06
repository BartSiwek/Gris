#include <gris/graphics/vulkan/VulkanDeviceResource.h>

#include <gris/graphics/vulkan/VulkanDevice.h>

#include <gris/assert.h>

// -------------------------------------------------------------------------------------------------

VulkanDeviceResource::VulkanDeviceResource(VulkanDevice* device) : m_parentDevice(device)
{
    GRIS_ALAWYS_ASSERT(m_parentDevice != nullptr, "Parent device cannot be null");
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const VulkanDevice& VulkanDeviceResource::ParentDevice() const
{
    return *m_parentDevice;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] VulkanDevice& VulkanDeviceResource::ParentDevice()
{
    return *m_parentDevice;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::Device& VulkanDeviceResource::DeviceHandle() const
{
    return m_parentDevice->DeviceHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Device& VulkanDeviceResource::DeviceHandle()
{
    return m_parentDevice->DeviceHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const VulkanAllocator& VulkanDeviceResource::Allocator() const
{
    return m_parentDevice->Allocator();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] VulkanAllocator& VulkanDeviceResource::Allocator()
{
    return m_parentDevice->Allocator();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::DescriptorPool& VulkanDeviceResource::DescriptorPoolHandle() const
{
    return m_parentDevice->DescriptorPoolHandle();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::DescriptorPool& VulkanDeviceResource::DescriptorPoolHandle()
{
    return m_parentDevice->DescriptorPoolHandle();
}
