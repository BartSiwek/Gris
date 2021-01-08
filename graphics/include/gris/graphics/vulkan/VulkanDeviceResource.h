#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

namespace Gris::Graphics::Vulkan
{

class VulkanDevice;
class VulkanAllocator;

class VulkanDeviceResource
{
public:
    virtual ~VulkanDeviceResource() = default;

protected:
    explicit VulkanDeviceResource(VulkanDevice * device);

    VulkanDeviceResource(const VulkanDeviceResource & other) = default;
    VulkanDeviceResource & operator=(const VulkanDeviceResource & other) = default;

    VulkanDeviceResource(VulkanDeviceResource && other) = default;
    VulkanDeviceResource & operator=(VulkanDeviceResource && other) = default;

    [[nodiscard]] const VulkanDevice & ParentDevice() const;
    [[nodiscard]] VulkanDevice & ParentDevice();

    [[nodiscard]] const vk::Device & DeviceHandle() const;
    [[nodiscard]] vk::Device & DeviceHandle();

    [[nodiscard]] const VulkanAllocator & Allocator() const;
    [[nodiscard]] VulkanAllocator & Allocator();

    [[nodiscard]] const vk::DescriptorPool & DescriptorPoolHandle() const;
    [[nodiscard]] vk::DescriptorPool & DescriptorPoolHandle();

private:
    VulkanDevice * m_parentDevice = nullptr;
};

}  // namespace Gris::Graphics::Vulkan
