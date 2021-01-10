#pragma once

#include <gris/graphics/vulkan/VulkanAllocation.h>

#include <gris/graphics/vulkan/vulkan_headers.h>

namespace Gris::Graphics::Vulkan
{

class VulkanAllocator
{
public:
    VulkanAllocator();

    explicit VulkanAllocator(VmaAllocator allocator);

    VulkanAllocator(const VulkanAllocator & other) = delete;
    VulkanAllocator & operator=(const VulkanAllocator & other) = delete;

    VulkanAllocator(VulkanAllocator && other) noexcept;
    VulkanAllocator & operator=(VulkanAllocator && other) noexcept;

    ~VulkanAllocator();

    [[nodiscard]] VulkanAllocation AllocateMemory(const vk::Buffer buffer, const VmaAllocationCreateInfo & allocationCreateInfo);
    [[nodiscard]] VulkanAllocation AllocateMemory(const vk::Image image, const VmaAllocationCreateInfo & allocationCreateInfo);

    void FreeMemory(const VmaAllocation & allocation);

    void Bind(const vk::Buffer & buffer, const VulkanAllocation & allocation);
    void Bind(const vk::Image & image, const VulkanAllocation & allocation);

    [[nodiscard]] void * Map(const VulkanAllocation & allocation) const;
    void Unmap(const VulkanAllocation & allocation) const;

private:
    VmaAllocator m_allocator = VK_NULL_HANDLE;
};

}  // namespace Gris::Graphics::Vulkan