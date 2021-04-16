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
    , m_allocation(std::exchange(other.m_allocation, static_cast<decltype(m_allocation)>(nullptr)))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocation & Gris::Graphics::Vulkan::Allocation::operator=(Allocation && other) noexcept
{
    if (this != &other)
    {
        ReleaseResources();

        ChildObject::operator=(std::move(static_cast<ChildObject &&>(other)));
        m_allocation = std::exchange(other.m_allocation, static_cast<decltype(m_allocation)>(nullptr));
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocation::~Allocation()
{
    ReleaseResources();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Allocation::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::Allocation::IsValid() const
{
    return IsParentValid() && m_allocation != nullptr;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Allocation::Reset()
{
    ReleaseResources();
    ResetParent();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Allocation::ReleaseResources()
{
    if (m_allocation != nullptr)
    {
        Parent().FreeMemory(m_allocation);
        m_allocation = nullptr;
    }
}
