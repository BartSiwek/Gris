#include <gris/graphics/vulkan/VulkanAllocation.h>

#include <gris/graphics/vulkan/VulkanAllocator.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanAllocation::VulkanAllocation() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanAllocation::VulkanAllocation(vma::Allocation allocation, VulkanAllocator * owner) : m_allocation(std::move(allocation)), m_owner(owner)
{}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanAllocation::VulkanAllocation(VulkanAllocation&& other) noexcept : m_allocation(std::exchange(other.m_allocation, nullptr)), m_owner(std::exchange(other.m_owner, nullptr))
{}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanAllocation& Gris::Graphics::Vulkan::VulkanAllocation::operator=(VulkanAllocation&& other) noexcept
{
    if (this != &other)
    {
        m_allocation = std::exchange(other.m_allocation, nullptr);
        m_owner = std::exchange(other.m_owner, nullptr);
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanAllocation::~VulkanAllocation()
{
    Reset();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanAllocation::Reset()
{
    if (m_owner)
    {
        m_owner->FreeMemory(m_allocation);
        m_allocation = nullptr;
        m_owner = nullptr;
    }
}
