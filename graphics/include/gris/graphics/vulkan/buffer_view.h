#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

#include <gris/object_hierarchy.h>

#include <cstdint>

namespace Gris::Graphics::Vulkan
{

class Buffer;

class BufferView : private ChildObject<Buffer>
{
public:
    BufferView();

    BufferView(const ParentObject<Buffer> & buffer, uint32_t offset, uint32_t size);

    BufferView(const BufferView &) = delete;
    BufferView & operator=(const BufferView &) = delete;

    BufferView(BufferView &&) noexcept = default;
    BufferView & operator=(BufferView &&) noexcept = default;

    ~BufferView() override = default;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const vk::Buffer & BufferHandle() const;

    [[nodiscard]] uint32_t Offset() const;

    [[nodiscard]] uint32_t Size() const;

    void Reset();

private:
    uint32_t m_offset = 0;
    uint32_t m_size = 0;
};

}  // namespace Gris::Graphics::Vulkan