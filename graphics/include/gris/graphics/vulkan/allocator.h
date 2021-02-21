#pragma once

#include <gris/graphics/vulkan/allocation.h>

#include <gris/graphics/vulkan/vulkan_headers.h>

#include <gris/object_hierarchy.h>

namespace Gris::Graphics::Vulkan
{

class Allocator : public ParentObject<Allocator>
{
public:
    Allocator();

    explicit Allocator(VmaAllocator allocator);

    Allocator(const Allocator &) = delete;
    Allocator & operator=(const Allocator &) = delete;

    Allocator(Allocator && other) noexcept;
    Allocator & operator=(Allocator && other) noexcept;

    virtual ~Allocator();

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] Allocation AllocateMemory(const vk::Buffer buffer, const VmaAllocationCreateInfo & allocationCreateInfo);
    [[nodiscard]] Allocation AllocateMemory(const vk::Image image, const VmaAllocationCreateInfo & allocationCreateInfo);

    void FreeMemory(const VmaAllocation & allocation);

    void Bind(const vk::Buffer & buffer, const Allocation & allocation);
    void Bind(const vk::Image & image, const Allocation & allocation);

    [[nodiscard]] void * Map(const Allocation & allocation) const;
    void Unmap(const Allocation & allocation) const;

    void Reset();

private:
    VmaAllocator m_allocator = VK_NULL_HANDLE;
};

}  // namespace Gris::Graphics::Vulkan