#include <gris/graphics/vulkan/buffer_view.h>

#include <gris/graphics/vulkan/buffer.h>

#include <gris/object_hierarchy.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::BufferView::BufferView() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::BufferView::BufferView(const ParentObject<Buffer> & buffer, uint32_t offset, uint32_t size)
    : ChildObject(buffer)
    , m_offset(offset)
    , m_size(size)
{
    GRIS_ALWAYS_ASSERT(m_size > 0, "Size must be greater than zero");
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::BufferView::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::BufferView::IsValid() const
{
    return ChildObject::IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::Buffer & Gris::Graphics::Vulkan::BufferView::BufferHandle() const
{
    return Parent().BufferHandle();
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

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::BufferView::Reset()
{
    m_size = 0;
    m_offset = 0;

    ResetParent();   
}
