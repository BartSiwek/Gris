#include <gris/graphics/vulkan/allocation.h>

#include <gris/graphics/vulkan/allocator.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocation::Allocation() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocation::Allocation(VmaAllocation allocation, const ParentObject<Allocator> & owner)
    : ChildObject(owner)
    , m_allocation(allocation)
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocation::Allocation(Allocation && other) noexcept
    : ChildObject(std::move(other)) 
    , m_allocation(std::exchange(other.m_allocation, static_cast<decltype(m_allocation)>(VK_NULL_HANDLE)))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocation & Gris::Graphics::Vulkan::Allocation::operator=(Allocation && other) noexcept
{
    if (this != &other)
    {
        Reset();

        ChildObject::operator=(std::move(other));
        m_allocation = std::exchange(other.m_allocation, static_cast<decltype(m_allocation)>(VK_NULL_HANDLE));
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocation::~Allocation()
{
    Reset();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocation::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::Allocation::IsValid() const
{
    return ChildObject::IsValid() && m_allocation != VK_NULL_HANDLE;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Allocation::Reset()
{
    if (IsValid())
    {
        Parent().FreeMemory(m_allocation);
        m_allocation = VK_NULL_HANDLE;
        ResetParent();
    }
}
