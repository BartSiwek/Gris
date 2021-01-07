#include <gris/graphics/vulkan/VulkanTexture.h>

#include <gris/graphics/vulkan/VulkanAllocator.h>
#include <gris/graphics/vulkan/VulkanEngineException.h>

Gris::Graphics::Vulkan::VulkanTexture::VulkanTexture(VulkanDevice* device, uint32_t width, uint32_t height, uint32_t mipLevels, vk::SampleCountFlagBits numSamples, vk::Format format, vk::ImageTiling tiling, const vk::ImageUsageFlags & usage, const vk::MemoryPropertyFlags & properties) : VulkanDeviceResource(device), m_mipLevels(mipLevels)
{
    auto const imageInfo = vk::ImageCreateInfo(
        {},
        vk::ImageType::e2D,
        format,
        vk::Extent3D(width, height, 1),
        m_mipLevels,
        1,
        numSamples,
        tiling,
        usage,
        vk::SharingMode::eExclusive,
        {},
        vk::ImageLayout::eUndefined
    );

    auto createImageResult = DeviceHandle().createImageUnique(imageInfo);
    if (createImageResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating image", createImageResult);

    m_image = std::move(createImageResult.value);

    auto const allocationInfo = vma::AllocationCreateInfo(vma::AllocationCreateFlags(), vma::MemoryUsage::eUnknown, properties, {}, 0, {}, nullptr);
    m_imageMemory = Allocator().AllocateMemory(m_image.get(), allocationInfo);

    Allocator().Bind(m_image.get(), m_imageMemory);
}

// TODO: Do this better
[[nodiscard]] const vk::Image& Gris::Graphics::Vulkan::VulkanTexture::ImageHandle() const
{
    return m_image.get();
}

// TODO: Do this better
[[nodiscard]] vk::Image& Gris::Graphics::Vulkan::VulkanTexture::ImageHandle()
{
    return m_image.get();
}
