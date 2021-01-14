#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

#include <cstdint>

namespace Gris::Graphics::Vulkan
{

class VulkanInputLayout
{
public:
    VulkanInputLayout() = default;

    void AddBinding(uint32_t binding, uint32_t stride, vk::VertexInputRate inputRate);
    void AddAttributeDescription(uint32_t location, uint32_t binding, vk::Format format, uint32_t offset);

    // TODO: Do this better
    [[nodiscard]] const std::vector<vk::VertexInputBindingDescription> & BindingDescription() const;
    [[nodiscard]] const std::vector<vk::VertexInputAttributeDescription> & AttributeDescriptions() const;

private:
    std::vector<vk::VertexInputBindingDescription> m_bindings = {};
    std::vector<vk::VertexInputAttributeDescription> m_attributes = {};
};

}  // namespace Gris::Graphics::Vulkan
