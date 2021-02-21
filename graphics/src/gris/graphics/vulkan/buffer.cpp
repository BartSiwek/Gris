#include <gris/graphics/vulkan/buffer.h>

#include <gris/graphics/vulkan/allocator.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Buffer::Buffer() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Buffer::Buffer(const ParentObject<Device> & device, vk::DeviceSize size, const vk::BufferUsageFlags & usage, const vk::MemoryPropertyFlags & properties)
    : DeviceResource(device)
    , ParentObject()
{
    auto const bufferInfo = vk::BufferCreateInfo{}
                                .setSize(size)
                                .setUsage(usage)
                                .setSharingMode(vk::SharingMode::eExclusive);

    auto createBufferResult = DeviceHandle().createBuffer(bufferInfo, nullptr, Dispatch());
    if (createBufferResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating buffer", createBufferResult);
    }

    m_buffer = createBufferResult.value;

    auto allocationInfo = VmaAllocationCreateInfo{};
    allocationInfo.flags = {};
    allocationInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;
    allocationInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(properties);
    allocationInfo.preferredFlags = {};
    allocationInfo.memoryTypeBits = 0;
    allocationInfo.pool = {};
    allocationInfo.pUserData = nullptr;
    m_bufferMemory = AllocatorHandle().AllocateMemory(m_buffer, allocationInfo);

    AllocatorHandle().Bind(m_buffer, m_bufferMemory);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Buffer::Buffer(Buffer && other) noexcept
    : DeviceResource(std::move(other))
    , ParentObject(std::move(other))
    , m_buffer(std::exchange(other.m_buffer, {}))
    , m_bufferMemory(std::exchange(other.m_bufferMemory, {}))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Buffer & Gris::Graphics::Vulkan::Buffer::operator=(Buffer && other) noexcept
{
    if (this != &other)
    {
        ReleaseResources();

        m_buffer = std::exchange(other.m_buffer, {});
        m_bufferMemory = std::exchange(other.m_bufferMemory, {});
        ParentObject::operator=(std::move(other));
        DeviceResource::operator=(std::move(other));
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Buffer::~Buffer()
{
    ReleaseResources();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Buffer::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::Buffer::IsValid() const
{
    return DeviceResource::IsValid() && static_cast<bool>(m_buffer);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::Buffer & Gris::Graphics::Vulkan::Buffer::BufferHandle() const
{
    return m_buffer;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Buffer & Gris::Graphics::Vulkan::Buffer::BufferHandle()
{
    return m_buffer;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Buffer::SetData(const void * const data, size_t size)
{
    auto * const memoryPtr = AllocatorHandle().Map(m_bufferMemory);
    memcpy(memoryPtr, data, size);
    AllocatorHandle().Unmap(m_bufferMemory);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Buffer::Reset()
{
    if (m_bufferMemory)
    {
        m_bufferMemory.Reset();
    }

    ReleaseResources();
    ResetParent();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Buffer::ReleaseResources()
{
    if (m_buffer)
    {
        DeviceHandle().destroyBuffer(m_buffer, nullptr, Dispatch());
        m_buffer = nullptr;
    }
}
