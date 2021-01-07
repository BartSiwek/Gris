#include <gris/graphics/vulkan/VulkanBuffer.h>

#include <gris/graphics/vulkan/VulkanAllocator.h>
#include <gris/graphics/vulkan/VulkanEngineException.h>

Gris::Graphics::Vulkan::VulkanBuffer::VulkanBuffer(VulkanDevice * device, vk::DeviceSize size, const vk::BufferUsageFlags & usage, const vk::MemoryPropertyFlags & properties)
    : VulkanDeviceResource(device)
{
    auto const bufferInfo = vk::BufferCreateInfo({}, size, usage, vk::SharingMode::eExclusive);

    auto createBufferResult = DeviceHandle().createBufferUnique(bufferInfo);
    if (createBufferResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating buffer", createBufferResult);

    m_buffer = std::move(createBufferResult.value);

    auto const allocationInfo = vma::AllocationCreateInfo(vma::AllocationCreateFlags(), vma::MemoryUsage::eUnknown, properties, {}, 0, {}, nullptr);
    m_bufferMemory = Allocator().AllocateMemory(m_buffer.get(), allocationInfo);

    Allocator().Bind(m_buffer.get(), m_bufferMemory);
}

// TODO: Do this via context
[[nodiscard]] const vk::Buffer & Gris::Graphics::Vulkan::VulkanBuffer::BufferHandle() const
{
    return m_buffer.get();
}

// TODO: Do this via context
[[nodiscard]] vk::Buffer & Gris::Graphics::Vulkan::VulkanBuffer::BufferHandle()
{
    return m_buffer.get();
}

void Gris::Graphics::Vulkan::VulkanBuffer::SetData(const void * const data, size_t size)
{
    auto * const memoryPtr = Allocator().Map(m_bufferMemory);
    memcpy(memoryPtr, data, size);
    Allocator().Unmap(m_bufferMemory);
}
