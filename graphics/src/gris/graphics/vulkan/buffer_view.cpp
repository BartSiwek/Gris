#include <gris/graphics/vulkan/buffer_view.h>

#include <gris/graphics/vulkan/buffer.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::BufferView::BufferView(const Buffer * buffer, uint32_t offset, uint32_t size)
    : m_buffer(buffer)
    , m_offset(offset)
    , m_size(size)
{
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::Buffer & Gris::Graphics::Vulkan::BufferView::BufferHandle() const
{
    return m_buffer->BufferHandle();
}

// -------------------------------------------------------------------------------------------------

uint32_t Gris::Graphics::Vulkan::BufferView::Offset() const
{
    return m_offset;
}

// -------------------------------------------------------------------------------------------------

uint32_t Gris::Graphics::Vulkan::BufferView::Size() const
{
    return m_size;
}
