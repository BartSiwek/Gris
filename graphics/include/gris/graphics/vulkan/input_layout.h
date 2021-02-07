#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

#include <cstdint>

namespace Gris::Graphics::Vulkan
{

class InputLayout
{
public:
    InputLayout() = default;

    InputLayout(const InputLayout &) = default;
    InputLayout & operator=(const InputLayout &) = default;

    InputLayout(InputLayout &&) noexcept = default;
    InputLayout & operator=(InputLayout &&) noexcept = default;

    ~InputLayout() = default;

    void AddBinding(uint32_t binding, uint32_t stride, vk::VertexInputRate inputRate);
    void AddAttributeDescription(uint32_t location, uint32_t binding, vk::Format format, uint32_t offset);

    [[nodiscard]] const std::vector<vk::VertexInputBindingDescription> & BindingDescription() const;
    [[nodiscard]] const std::vector<vk::VertexInputAttributeDescription> & AttributeDescriptions() const;

private:
    std::vector<vk::VertexInputBindingDescription> m_bindings = {};
    std::vector<vk::VertexInputAttributeDescription> m_attributes = {};
};

}  // namespace Gris::Graphics::Vulkan
