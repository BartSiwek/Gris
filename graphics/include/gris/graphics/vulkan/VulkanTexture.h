#pragma once

#include "VulkanDeviceResource.h"
#include "VulkanAllocation.h"

namespace Gris::Graphics::Vulkan
{

class VulkanTexture : public VulkanDeviceResource
{
public:
    VulkanTexture(VulkanDevice* device, uint32_t width, uint32_t height, uint32_t mipLevels, vk::SampleCountFlagBits numSamples, vk::Format format, vk::ImageTiling tiling, const vk::ImageUsageFlags& usage, const vk::MemoryPropertyFlags& properties);

    [[nodiscard]] uint32_t MipLevels() const
    {
        return m_mipLevels;
    }

    // TODO: Do this better via context
    [[nodiscard]] const vk::Image& ImageHandle() const;
    [[nodiscard]] vk::Image& ImageHandle();

private:
    vk::UniqueImage m_image = {};
    VulkanAllocation m_imageMemory = {};

    uint32_t m_mipLevels = 1;
};

}  // namespace Gris::Graphics::Vulkan
