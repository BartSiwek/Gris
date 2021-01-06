#pragma once

#include "VulkanAllocation.h"

class VulkanAllocator
{
public:
    VulkanAllocator();

    explicit VulkanAllocator(vma::Allocator allocator);

    VulkanAllocator(const VulkanAllocator& other) = delete;
    VulkanAllocator& operator=(const VulkanAllocator& other) = delete;

    VulkanAllocator(VulkanAllocator&& other) noexcept;
    VulkanAllocator& operator=(VulkanAllocator&& other) noexcept;

    ~VulkanAllocator();

    [[nodiscard]] VulkanAllocation AllocateMemory(const vk::Buffer buffer, const vma::AllocationCreateInfo& allocationCreateInfo);
    [[nodiscard]] VulkanAllocation AllocateMemory(const vk::Image buffer, const vma::AllocationCreateInfo& allocationCreateInfo);

    void FreeMemory(const vma::Allocation& allocation);

    void Bind(const vk::Buffer& buffer, const VulkanAllocation& allocation);
    void Bind(const vk::Image& image, const VulkanAllocation& allocation);

    [[nodiscard]] void* Map(const VulkanAllocation& allocation) const;
    void Unmap(const VulkanAllocation& allocation) const;

private:
    vma::Allocator m_allocator = nullptr;
};