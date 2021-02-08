#include <gris/graphics/vulkan/allocation.h>

#include <gris/graphics/vulkan/allocator.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocation::Allocation() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocation::Allocation(VmaAllocation allocation, Allocator * owner)
    : m_allocation(allocation)
    , m_owner(owner)
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocation::Allocation(Allocation && other) noexcept
    : m_allocation(std::exchange(other.m_allocation, static_cast<decltype(m_allocation)>(VK_NULL_HANDLE)))
    , m_owner(std::exchange(other.m_owner, nullptr))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocation & Gris::Graphics::Vulkan::Allocation::operator=(Allocation && other) noexcept
{
    if (this != &other)
    {
        m_allocation = std::exchange(other.m_allocation, static_cast<decltype(m_allocation)>(VK_NULL_HANDLE));
        m_owner = std::exchange(other.m_owner, nullptr);
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
    return m_owner != nullptr && m_allocation != VK_NULL_HANDLE;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Allocation::Reset()
{
    if (IsValid())
    {
        m_owner->FreeMemory(m_allocation);
        m_allocation = VK_NULL_HANDLE;
        m_owner = nullptr;
    }
}
