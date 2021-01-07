#pragma once

#include "VulkanDeviceResource.h"

namespace Gris::Graphics::Vulkan
{

class VulkanTexture;

class VulkanTextureView : public VulkanDeviceResource
{
public:
    VulkanTextureView(VulkanDevice * device,
                      const VulkanTexture & image,
                      vk::Format format,
                      const vk::ImageAspectFlags & aspectFlags,
                      uint32_t mipLevels);

    VulkanTextureView(VulkanDevice * device,
                      const vk::Image & image,
                      vk::Format format,
                      const vk::ImageAspectFlags & aspectFlags,
                      uint32_t mipLevels);

    // TODO: Do this better
    [[nodiscard]] const vk::ImageView & ImageViewHandle() const;
    [[nodiscard]] vk::ImageView & ImageViewHandle();

private:
    vk::UniqueImageView m_imageView;
};

}  // namespace Gris::Graphics::Vulkan
