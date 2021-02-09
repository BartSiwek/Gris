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

    Allocation(const Allocation &) = delete;
    Allocation & operator=(const Allocation &) = delete;

    Allocation(Allocation && other) noexcept;
    Allocation & operator=(Allocation && other) noexcept;

    ~Allocation();

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    void Reset();

private:
    friend class Allocator;

    VmaAllocation m_allocation = VK_NULL_HANDLE;
    Allocator * m_owner = nullptr;
};

}  // namespace Gris::Graphics::Vulkan
