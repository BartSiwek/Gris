#pragma once

#include <gris/graphics/vulkan/allocation.h>
#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Buffer : public DeviceResource
{
public:
    Buffer();

    Buffer(std::shared_ptr<DeviceSharedData> sharedData, vk::DeviceSize size, const vk::BufferUsageFlags & usage, const vk::MemoryPropertyFlags & properties);

    Buffer(const Buffer &) = delete;
    Buffer & operator=(const Buffer &) = delete;

    Buffer(Buffer &&) noexcept = default;
    Buffer & operator=(Buffer &&) noexcept = default;

    ~Buffer() = default;

    explicit operator bool() const;

    bool IsValid() const;

    [[nodiscard]] const vk::Buffer & BufferHandle() const;
    [[nodiscard]] vk::Buffer & BufferHandle();

    void SetData(const void * data, size_t size);

private:
    vk::UniqueBuffer m_buffer = {};
    Allocation m_bufferMemory = {};
};

}  // namespace Gris::Graphics::Vulkan
