#include <gris/graphics/vulkan/VulkanAllocation.h>

#include <gris/graphics/vulkan/VulkanAllocator.h>

// -------------------------------------------------------------------------------------------------

VulkanAllocation::VulkanAllocation() = default;

// -------------------------------------------------------------------------------------------------

VulkanAllocation::VulkanAllocation(vma::Allocation allocation, VulkanAllocator * owner) : m_allocation(std::move(allocation)), m_owner(owner)
{}

// -------------------------------------------------------------------------------------------------

VulkanAllocation::VulkanAllocation(VulkanAllocation&& other) noexcept : m_allocation(std::exchange(other.m_allocation, nullptr)), m_owner(std::exchange(other.m_owner, nullptr))
{}

// -------------------------------------------------------------------------------------------------

VulkanAllocation& VulkanAllocation::operator=(VulkanAllocation&& other) noexcept
{
    if (this != &other)
    {
        m_allocation = std::exchange(other.m_allocation, nullptr);
        m_owner = std::exchange(other.m_owner, nullptr);
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

VulkanAllocation::~VulkanAllocation()
{
    Reset();
}

// -------------------------------------------------------------------------------------------------

void VulkanAllocation::Reset()
{
    if (m_owner)
    {
        m_owner->FreeMemory(m_allocation);
        m_allocation = nullptr;
        m_owner = nullptr;
    }
}
