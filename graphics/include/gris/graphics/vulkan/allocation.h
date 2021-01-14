#pragma once

#include <gris/graphics/vulkan/vma_headers.h>

namespace Gris::Graphics::Vulkan
{

class Allocator;

class Allocation
{
public:
    Allocation();

    Allocation(VmaAllocation allocation, Allocator * owner);

    Allocation(const Allocation & other) = delete;
    Allocation & operator=(const Allocation & other) = delete;

    Allocation(Allocation && other) noexcept;
    Allocation & operator=(Allocation && other) noexcept;

    ~Allocation();

    void Reset();

private:
    friend class Allocator;

    VmaAllocation m_allocation = VK_NULL_HANDLE;
    Allocator * m_owner = nullptr;
};

}  // namespace Gris::Graphics::Vulkan
