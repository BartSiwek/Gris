#pragma once

#include "VulkanDeviceResource.h"
#include "VulkanAllocation.h"

namespace Gris::Graphics::Vulkan
{

class VulkanBuffer : public VulkanDeviceResource
{
public:
    VulkanBuffer(VulkanDevice* device, vk::DeviceSize size, const vk::BufferUsageFlags& usage, const vk::MemoryPropertyFlags& properties);
    ~VulkanBuffer() = default;

    VulkanBuffer(const VulkanBuffer& other) = delete;
    VulkanBuffer& operator=(const VulkanBuffer& other) = delete;

    VulkanBuffer(VulkanBuffer&& other) = default;
    VulkanBuffer& operator=(VulkanBuffer&& other) = default;

    // TODO: Do this via context
    [[nodiscard]] const vk::Buffer& BufferHandle() const;   
    [[nodiscard]] vk::Buffer& BufferHandle();

    void SetData(const void* data, size_t size);

private:
    vk::UniqueBuffer m_buffer = {};
    VulkanAllocation m_bufferMemory = {};
};

}  // namespace Gris::Graphics::Vulkan
