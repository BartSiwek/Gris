﻿#include <gris/graphics/vulkan/VulkanTextureView.h>

#include <gris/graphics/vulkan/VulkanDevice.h>
#include <gris/graphics/vulkan/VulkanEngineException.h>
#include <gris/graphics/vulkan/VulkanTexture.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanTextureView::VulkanTextureView(VulkanDevice * device,
                                                             const VulkanTexture & image,
                                                             vk::Format format,
                                                             const vk::ImageAspectFlags & aspectFlags,
                                                             uint32_t mipLevels)
    : VulkanTextureView(device, image.ImageHandle(), format, aspectFlags, mipLevels)
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanTextureView::VulkanTextureView(VulkanDevice * device,
                                                             const vk::Image & image,
                                                             vk::Format format,
                                                             const vk::ImageAspectFlags & aspectFlags,
                                                             uint32_t mipLevels)
    : VulkanDeviceResource(device)
{
    const vk::ImageViewCreateInfo viewInfo(
        {},
        image,
        vk::ImageViewType::e2D,
        format,
        {},
        vk::ImageSubresourceRange(
            aspectFlags,
            0,
            mipLevels,
            0,
            1));

    auto createImageViewResult = DeviceHandle().createImageViewUnique(viewInfo);
    if (createImageViewResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating image view", createImageViewResult);

    m_imageView = std::move(createImageViewResult.value);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::ImageView & Gris::Graphics::Vulkan::VulkanTextureView::ImageViewHandle() const
{
    return m_imageView.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::ImageView & Gris::Graphics::Vulkan::VulkanTextureView::ImageViewHandle()
{
    return m_imageView.get();
}
