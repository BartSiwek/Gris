#pragma once

#include <gris/graphics/vulkan/vma_headers.h>

#include <gris/object_hierarchy.h>

namespace Gris::Graphics::Vulkan
{

class Allocator;

class Allocation : private ChildObject<Allocator>
{
public:
    Allocation();

    Allocation(VmaAllocation allocation, const ParentObject<Allocator> & owner);

    Allocation(const Allocation &) = delete;
    Allocation & operator=(const Allocation &) = delete;

    Allocation(Allocation && other) noexcept;
    Allocation & operator=(Allocation && other) noexcept;

    virtual ~Allocation();

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    void Reset();

private:
    friend class Allocator;

    VmaAllocation m_allocation = VK_NULL_HANDLE;
};

}  // namespace Gris::Graphics::Vulkan
