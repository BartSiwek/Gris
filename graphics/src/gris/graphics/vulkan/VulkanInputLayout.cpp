#include <gris/graphics/vulkan/VulkanInputLayout.h>

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanInputLayout::AddBinding(uint32_t binding, uint32_t stride, vk::VertexInputRate inputRate)
{
    m_bindings.emplace_back(binding, stride, inputRate);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanInputLayout::AddAttributeDescription(uint32_t location, uint32_t binding, vk::Format format, uint32_t offset)
{
    m_attributes.emplace_back(location, binding, format, offset);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const std::vector<vk::VertexInputBindingDescription>& Gris::Graphics::Vulkan::VulkanInputLayout::BindingDescription() const
{
    return m_bindings;
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const std::vector<vk::VertexInputAttributeDescription>& Gris::Graphics::Vulkan::VulkanInputLayout::AttributeDescriptions() const
{
    return m_attributes;
}
