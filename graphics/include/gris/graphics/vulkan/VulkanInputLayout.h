#pragma once

#define VULKAN_HPP_TYPESAFE_CONVERSION
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

#include <cstdint>

class VulkanInputLayout
{
public:
    VulkanInputLayout() = default;

    void AddBinding(uint32_t binding, uint32_t stride, vk::VertexInputRate inputRate);
    void AddAttributeDescription(uint32_t location, uint32_t binding, vk::Format format, uint32_t offset);

    // TODO: Do this better
    [[nodiscard]] const std::vector<vk::VertexInputBindingDescription>& BindingDescription() const;
    [[nodiscard]] const std::vector<vk::VertexInputAttributeDescription>& AttributeDescriptions() const;

private:
    std::vector<vk::VertexInputBindingDescription> m_bindings = {};
    std::vector<vk::VertexInputAttributeDescription> m_attributes = {};
};
