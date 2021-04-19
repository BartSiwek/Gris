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

    ~Allocator() override;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] Allocation AllocateMemory(vk::Buffer buffer, const VmaAllocationCreateInfo & allocationCreateInfo) const;
    [[nodiscard]] Allocation AllocateMemory(vk::Image image, const VmaAllocationCreateInfo & allocationCreateInfo) const;

    void FreeMemory(const VmaAllocation & allocation) const;

    void Bind(const vk::Buffer & buffer, const Allocation & allocation) const;
    void Bind(const vk::Image & image, const Allocation & allocation) const;

    [[nodiscard]] void * Map(const Allocation & allocation) const;
    void Unmap(const Allocation & allocation) const;

    void Reset();

private:
    void ReleaseResources();

    VmaAllocator m_allocator = nullptr;
};

}  // namespace Gris::Graphics::Vulkan