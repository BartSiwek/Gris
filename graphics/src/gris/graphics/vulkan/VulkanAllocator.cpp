#include <gris/graphics/vulkan/VulkanAllocator.h>

#include <gris/graphics/vulkan/VulkanEngineException.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanAllocator::VulkanAllocator() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanAllocator::VulkanAllocator(VmaAllocator allocator)
    : m_allocator(std::move(allocator))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanAllocator::VulkanAllocator(VulkanAllocator && other) noexcept
    : m_allocator(std::exchange(other.m_allocator, static_cast<decltype(m_allocator)>(VK_NULL_HANDLE)))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanAllocator & Gris::Graphics::Vulkan::VulkanAllocator::operator=(VulkanAllocator && other) noexcept
{
    if (this != &other)
        m_allocator = std::exchange(other.m_allocator, static_cast<decltype(m_allocator)>(VK_NULL_HANDLE));

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanAllocator::~VulkanAllocator()
{
    if (m_allocator)
        vmaDestroyAllocator(m_allocator);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanAllocation Gris::Graphics::Vulkan::VulkanAllocator::AllocateMemory(const vk::Buffer buffer, const VmaAllocationCreateInfo & allocationCreateInfo)
{
    VmaAllocation allocation;
    auto const createBufferResult = static_cast<vk::Result>(vmaAllocateMemoryForBuffer(m_allocator, static_cast<VkBuffer>(buffer), &allocationCreateInfo, &allocation, nullptr));
    if (createBufferResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error allocating buffer from VMA", vk::to_string(createBufferResult));

    return VulkanAllocation(allocation, this);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanAllocation Gris::Graphics::Vulkan::VulkanAllocator::AllocateMemory(const vk::Image image, const VmaAllocationCreateInfo & allocationCreateInfo)
{
    VmaAllocation allocation;
    auto const createImageResult = static_cast<vk::Result>(vmaAllocateMemoryForImage(m_allocator, static_cast<VkImage>(image), &allocationCreateInfo, &allocation, nullptr));
    if (createImageResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error allocating image from VMA", vk::to_string(createImageResult));

    return VulkanAllocation(allocation, this);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanAllocator::FreeMemory(const VmaAllocation & allocation)
{
    vmaFreeMemory(m_allocator, allocation);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanAllocator::Bind(const vk::Buffer & buffer, const VulkanAllocation & allocation)
{
    auto const bindResult = static_cast<vk::Result>(vmaBindBufferMemory(m_allocator, allocation.m_allocation, static_cast<VkBuffer>(buffer)));
    if (bindResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error binding buffer memory", vk::to_string(bindResult));
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanAllocator::Bind(const vk::Image & image, const VulkanAllocation & allocation)
{
    auto const bindResult = static_cast<vk::Result>(vmaBindImageMemory(m_allocator, allocation.m_allocation, static_cast<VkImage>(image)));
    if (bindResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error binding buffer memory", vk::to_string(bindResult));
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] void * Gris::Graphics::Vulkan::VulkanAllocator::Map(const VulkanAllocation & allocation) const
{
    void* data;
    auto const mapResult = static_cast<vk::Result>(vmaMapMemory(m_allocator, allocation.m_allocation, &data));
    if (mapResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error mapping memory", vk::to_string(mapResult));

    return data;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanAllocator::Unmap(const VulkanAllocation & allocation) const
{
    vmaUnmapMemory(m_allocator, allocation.m_allocation);
}
