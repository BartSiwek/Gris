#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

#include <cstdint>

namespace Gris::Graphics::Vulkan
{

class Buffer;

class BufferView
{
public:
    BufferView();

    BufferView(const Buffer * buffer, uint32_t offset, uint32_t size);

    BufferView(const BufferView &) = default;
    BufferView & operator=(const BufferView &) = default;

    BufferView(BufferView &&) noexcept = default;
    BufferView & operator=(BufferView &&) noexcept = default;

    ~BufferView() = default;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const vk::Buffer & BufferHandle() const;

    uint32_t Offset() const;

    uint32_t Size() const;

private:
    const Buffer * m_buffer = nullptr;
    uint32_t m_offset = 0;
    uint32_t m_size = 0;
};

}  // namespace Gris::Graphics::Vulkan