#include <gris/graphics/vulkan/input_layout.h>

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::InputLayout::AddBinding(uint32_t binding, uint32_t stride, vk::VertexInputRate inputRate)
{
    m_bindings.emplace_back(binding, stride, inputRate);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::InputLayout::AddAttributeDescription(uint32_t location, uint32_t binding, vk::Format format, uint32_t offset)
{
    m_attributes.emplace_back(location, binding, format, offset);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const std::vector<vk::VertexInputBindingDescription> & Gris::Graphics::Vulkan::InputLayout::BindingDescription() const
{
    return m_bindings;
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const std::vector<vk::VertexInputAttributeDescription> & Gris::Graphics::Vulkan::InputLayout::AttributeDescriptions() const
{
    return m_attributes;
}
