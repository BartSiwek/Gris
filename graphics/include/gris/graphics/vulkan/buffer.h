#pragma once

#include <gris/graphics/vulkan/allocation.h>
#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Buffer : public DeviceResource
{
public:
    Buffer(Device * device, vk::DeviceSize size, const vk::BufferUsageFlags & usage, const vk::MemoryPropertyFlags & properties);
    ~Buffer() = default;

    Buffer(const Buffer & other) = delete;
    Buffer & operator=(const Buffer & other) = delete;

    Buffer(Buffer && other) = default;
    Buffer & operator=(Buffer && other) = default;

    [[nodiscard]] const vk::Buffer & BufferHandle() const;
    [[nodiscard]] vk::Buffer & BufferHandle();

    void SetData(const void * data, size_t size);

private:
    vk::UniqueBuffer m_buffer = {};
    Allocation m_bufferMemory = {};
};

}  // namespace Gris::Graphics::Vulkan
