#include <gris/graphics/vulkan/VulkanBufferView.h>

#include <gris/graphics/vulkan/VulkanBuffer.h>

// -------------------------------------------------------------------------------------------------

VulkanBufferView::VulkanBufferView(VulkanBuffer* buffer, uint32_t offset, uint32_t size) : m_buffer(buffer), m_offset(offset), m_size(size)
{}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::Buffer& VulkanBufferView::BufferHandle() const
{
    return m_buffer->BufferHandle();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::Buffer& VulkanBufferView::BufferHandle()
{
    return m_buffer->BufferHandle();
}

// -------------------------------------------------------------------------------------------------

uint32_t VulkanBufferView::Offset() const
{
    return m_offset;
}

// -------------------------------------------------------------------------------------------------

uint32_t VulkanBufferView::Size() const
{
    return m_size;
}
