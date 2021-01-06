#include <gris/graphics/vulkan/VulkanAllocator.h>

#include <gris/graphics/vulkan/VulkanEngineException.h>

// -------------------------------------------------------------------------------------------------

VulkanAllocator::VulkanAllocator() = default;

// -------------------------------------------------------------------------------------------------

VulkanAllocator::VulkanAllocator(vma::Allocator allocator) : m_allocator(std::move(allocator))
{}

// -------------------------------------------------------------------------------------------------

VulkanAllocator::VulkanAllocator(VulkanAllocator && other) noexcept : m_allocator(std::exchange(other.m_allocator, nullptr))
{}

// -------------------------------------------------------------------------------------------------

VulkanAllocator& VulkanAllocator::operator=(VulkanAllocator && other) noexcept
{
    if (this != &other)
        m_allocator = std::exchange(other.m_allocator, nullptr);

    return *this;
}

// -------------------------------------------------------------------------------------------------

VulkanAllocator::~VulkanAllocator()
{
    if (m_allocator)
        m_allocator.destroy();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] VulkanAllocation VulkanAllocator::AllocateMemory(const vk::Buffer buffer, const vma::AllocationCreateInfo & allocationCreateInfo)
{
    auto const createBufferResult = m_allocator.allocateMemoryForBuffer(buffer, allocationCreateInfo);
    if (createBufferResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error allocating buffer from VMA", vk::to_string(createBufferResult.result));

    return VulkanAllocation(createBufferResult.value, this);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] VulkanAllocation VulkanAllocator::AllocateMemory(const vk::Image buffer, const vma::AllocationCreateInfo & allocationCreateInfo)
{
    auto const createBufferResult = m_allocator.allocateMemoryForImage(buffer, allocationCreateInfo);
    if (createBufferResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error allocating image from VMA", vk::to_string(createBufferResult.result));

    return VulkanAllocation(createBufferResult.value, this);
}

// -------------------------------------------------------------------------------------------------

void VulkanAllocator::FreeMemory(const vma::Allocation & allocation)
{
    m_allocator.freeMemory(allocation);
}

// -------------------------------------------------------------------------------------------------

void VulkanAllocator::Bind(const vk::Buffer & buffer, const VulkanAllocation & allocation)
{
    auto const bindResult = m_allocator.bindBufferMemory(allocation.m_allocation, buffer);
    if (bindResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error binding buffer memory", vk::to_string(bindResult));
}

// -------------------------------------------------------------------------------------------------

void VulkanAllocator::Bind(const vk::Image & image, const VulkanAllocation & allocation)
{
    auto const bindResult = m_allocator.bindImageMemory(allocation.m_allocation, image);
    if (bindResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error binding buffer memory", vk::to_string(bindResult));
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] void* VulkanAllocator::Map(const VulkanAllocation & allocation) const
{
    auto const mapResult = m_allocator.mapMemory(allocation.m_allocation);
    if (mapResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error mapping memory", vk::to_string(mapResult.result));

    return mapResult.value;
}

// -------------------------------------------------------------------------------------------------

void VulkanAllocator::Unmap(const VulkanAllocation & allocation) const
{
    m_allocator.unmapMemory(allocation.m_allocation);
}
