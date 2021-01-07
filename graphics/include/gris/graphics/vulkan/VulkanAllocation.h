#pragma once

#define VULKAN_HPP_TYPESAFE_CONVERSION
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vk_mem_alloc.hpp>

namespace Gris::Graphics::Vulkan
{

class VulkanAllocator;

class VulkanAllocation
{
public:
    VulkanAllocation();

    VulkanAllocation(vma::Allocation allocation, VulkanAllocator * owner);

    VulkanAllocation(const VulkanAllocation & other) = delete;
    VulkanAllocation & operator=(const VulkanAllocation & other) = delete;

    VulkanAllocation(VulkanAllocation && other) noexcept;
    VulkanAllocation & operator=(VulkanAllocation && other) noexcept;

    ~VulkanAllocation();

    void Reset();

private:
    friend class VulkanAllocator;

    vma::Allocation m_allocation = nullptr;
    VulkanAllocator * m_owner = nullptr;
};

}  // namespace Gris::Graphics::Vulkan
