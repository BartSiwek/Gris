#include <gris/graphics/vulkan/allocator.h>

#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocator::Allocator() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocator::Allocator(VmaAllocator allocator)
    : m_allocator(std::move(allocator))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocator::Allocator(Allocator && other) noexcept
    : m_allocator(std::exchange(other.m_allocator, static_cast<decltype(m_allocator)>(VK_NULL_HANDLE)))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocator & Gris::Graphics::Vulkan::Allocator::operator=(Allocator && other) noexcept
{
    if (this != &other)
        m_allocator = std::exchange(other.m_allocator, static_cast<decltype(m_allocator)>(VK_NULL_HANDLE));

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocator::~Allocator()
{
    if (m_allocator)
        vmaDestroyAllocator(m_allocator);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Allocation Gris::Graphics::Vulkan::Allocator::AllocateMemory(const vk::Buffer buffer, const VmaAllocationCreateInfo & allocationCreateInfo)
{
    VmaAllocation allocation;
    auto const createBufferResult = static_cast<vk::Result>(vmaAllocateMemoryForBuffer(m_allocator, static_cast<VkBuffer>(buffer), &allocationCreateInfo, &allocation, nullptr));
    if (createBufferResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error allocating buffer from VMA", vk::to_string(createBufferResult));

    return Allocation(allocation, this);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Allocation Gris::Graphics::Vulkan::Allocator::AllocateMemory(const vk::Image image, const VmaAllocationCreateInfo & allocationCreateInfo)
{
    VmaAllocation allocation;
    auto const createImageResult = static_cast<vk::Result>(vmaAllocateMemoryForImage(m_allocator, static_cast<VkImage>(image), &allocationCreateInfo, &allocation, nullptr));
    if (createImageResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error allocating image from VMA", vk::to_string(createImageResult));

    return Allocation(allocation, this);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Allocator::FreeMemory(const VmaAllocation & allocation)
{
    vmaFreeMemory(m_allocator, allocation);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Allocator::Bind(const vk::Buffer & buffer, const Allocation & allocation)
{
    auto const bindResult = static_cast<vk::Result>(vmaBindBufferMemory(m_allocator, allocation.m_allocation, static_cast<VkBuffer>(buffer)));
    if (bindResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error binding buffer memory", vk::to_string(bindResult));
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Allocator::Bind(const vk::Image & image, const Allocation & allocation)
{
    auto const bindResult = static_cast<vk::Result>(vmaBindImageMemory(m_allocator, allocation.m_allocation, static_cast<VkImage>(image)));
    if (bindResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error binding buffer memory", vk::to_string(bindResult));
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] void * Gris::Graphics::Vulkan::Allocator::Map(const Allocation & allocation) const
{
    void * data;
    auto const mapResult = static_cast<vk::Result>(vmaMapMemory(m_allocator, allocation.m_allocation, &data));
    if (mapResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error mapping memory", vk::to_string(mapResult));

    return data;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Allocator::Unmap(const Allocation & allocation) const
{
    vmaUnmapMemory(m_allocator, allocation.m_allocation);
}
