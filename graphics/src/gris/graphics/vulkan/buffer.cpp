#include <gris/graphics/vulkan/buffer.h>

#include <gris/graphics/vulkan/allocator.h>
#include <gris/graphics/vulkan/engine_exception.h>

Gris::Graphics::Vulkan::VulkanBuffer::VulkanBuffer(VulkanDevice * device, vk::DeviceSize size, const vk::BufferUsageFlags & usage, const vk::MemoryPropertyFlags & properties)
    : VulkanDeviceResource(device)
{
    auto const bufferInfo = vk::BufferCreateInfo({}, size, usage, vk::SharingMode::eExclusive);

    auto createBufferResult = DeviceHandle().createBufferUnique(bufferInfo);
    if (createBufferResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating buffer", createBufferResult);

    m_buffer = std::move(createBufferResult.value);

    auto allocationInfo = VmaAllocationCreateInfo{};
    allocationInfo.flags = {};
    allocationInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;
    allocationInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(properties);
    allocationInfo.preferredFlags = {};
    allocationInfo.memoryTypeBits = 0;
    allocationInfo.pool = {};
    allocationInfo.pUserData = nullptr;
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
