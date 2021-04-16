#pragma once

#include <gris/graphics/vulkan/allocation.h>
#include <gris/graphics/vulkan/device_resource.h>

#include <gris/object_hierarchy.h>

namespace Gris::Graphics::Vulkan
{

class Buffer : public DeviceResource, public ParentObject<Buffer>
{
public:
    Buffer();

    Buffer(const ParentObject<Device> & device, vk::DeviceSize size, const vk::BufferUsageFlags & usage, const vk::MemoryPropertyFlags & properties);

    Buffer(const Buffer &) = delete;
    Buffer & operator=(const Buffer &) = delete;

    Buffer(Buffer && other) noexcept;
    Buffer & operator=(Buffer && other) noexcept;

    ~Buffer() override;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const vk::Buffer & BufferHandle() const;
    [[nodiscard]] vk::Buffer & BufferHandle();

    void SetData(const void * data, size_t size);

    void Reset();

private:
    void ReleaseResources();

    vk::Buffer m_buffer = {};
    Allocation m_bufferMemory = {};
};

}  // namespace Gris::Graphics::Vulkan
