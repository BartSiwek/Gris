﻿#pragma once

#include <gris/graphics/vulkan/allocation.h>
#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Texture : public DeviceResource
{
public:
    Texture();

    Texture(std::shared_ptr<DeviceSharedData> sharedData,
            uint32_t width,
            uint32_t height,
            uint32_t mipLevels,
            vk::SampleCountFlagBits numSamples,
            vk::Format format,
            vk::ImageTiling tiling,
            const vk::ImageUsageFlags & usage,
            const vk::MemoryPropertyFlags & properties);

    Texture(const Texture &) = delete;
    Texture & operator=(const Texture &) = delete;

    Texture(Texture &&) noexcept = default;
    Texture & operator=(Texture &&) noexcept = default;

    ~Texture() = default;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] uint32_t MipLevels() const
    {
        return m_mipLevels;
    }

    [[nodiscard]] const vk::Image & ImageHandle() const;
    [[nodiscard]] vk::Image & ImageHandle();

private:
    vk::UniqueImage m_image = {};
    Allocation m_imageMemory = {};

    uint32_t m_mipLevels = 1;
};

}  // namespace Gris::Graphics::Vulkan
