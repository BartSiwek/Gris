﻿#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

#include <cstdint>

namespace Gris::Graphics::Vulkan
{

class VulkanBuffer;

class VulkanBufferView
{
public:
    VulkanBufferView(VulkanBuffer * buffer, uint32_t offset, uint32_t size);

    // TODO: Do this better
    [[nodiscard]] const vk::Buffer & BufferHandle() const;
    [[nodiscard]] vk::Buffer & BufferHandle();

    uint32_t Offset() const;

    uint32_t Size() const;

private:
    VulkanBuffer * m_buffer = nullptr;
    uint32_t m_offset = 0;
    uint32_t m_size = 0;
};

}  // namespace Gris::Graphics::Vulkan