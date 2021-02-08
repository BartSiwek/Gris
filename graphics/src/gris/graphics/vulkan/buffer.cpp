#include <gris/graphics/vulkan/buffer.h>

#include <gris/graphics/vulkan/allocator.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Buffer::Buffer() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Buffer::Buffer(std::shared_ptr<DeviceSharedData> sharedData, vk::DeviceSize size, const vk::BufferUsageFlags & usage, const vk::MemoryPropertyFlags & properties)
    : DeviceResource(sharedData)
{
    auto const bufferInfo = vk::BufferCreateInfo{}
                                .setSize(size)
                                .setUsage(usage)
                                .setSharingMode(vk::SharingMode::eExclusive);

    auto createBufferResult = DeviceHandle().createBufferUnique(bufferInfo, nullptr, Dispatch());
    if (createBufferResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating buffer", createBufferResult);
    }

    m_buffer = std::move(createBufferResult.value);

    auto allocationInfo = VmaAllocationCreateInfo{};
    allocationInfo.flags = {};
    allocationInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;
    allocationInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(properties);
    allocationInfo.preferredFlags = {};
    allocationInfo.memoryTypeBits = 0;
    allocationInfo.pool = {};
    allocationInfo.pUserData = nullptr;
    m_bufferMemory = AllocatorHandle().AllocateMemory(m_buffer.get(), allocationInfo);

    AllocatorHandle().Bind(m_buffer.get(), m_bufferMemory);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Buffer::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::Buffer::IsValid() const
{
    return DeviceResource::IsValid() && static_cast<bool>(m_buffer) && m_bufferMemory.IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::Buffer & Gris::Graphics::Vulkan::Buffer::BufferHandle() const
{
    return m_buffer.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Buffer & Gris::Graphics::Vulkan::Buffer::BufferHandle()
{
    return m_buffer.get();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Buffer::SetData(const void * const data, size_t size)
{
    auto * const memoryPtr = AllocatorHandle().Map(m_bufferMemory);
    memcpy(memoryPtr, data, size);
    AllocatorHandle().Unmap(m_bufferMemory);
}
