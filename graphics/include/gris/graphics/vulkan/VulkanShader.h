#pragma once

#include <gris/graphics/vulkan/VulkanDeviceResource.h>

namespace Gris::Graphics::Vulkan
{

class VulkanShader : public VulkanDeviceResource
{
public:
    VulkanShader(VulkanDevice * device, const std::vector<char> & code);

    // TODO: Do this better
    [[nodiscard]] const vk::ShaderModule & ModuleHandle() const;
    [[nodiscard]] vk::ShaderModule & ModuleHandle();

private:
    vk::UniqueShaderModule m_shaderModule = {};
};

}  // namespace Gris::Graphics::Vulkan
