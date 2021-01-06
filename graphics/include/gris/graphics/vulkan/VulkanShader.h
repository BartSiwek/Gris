#pragma once

#include "VulkanDeviceResource.h"

class VulkanShader : public VulkanDeviceResource
{
public:
    VulkanShader(VulkanDevice* device, const std::vector<char>& code);

    // TODO: Do this better
    [[nodiscard]] const vk::ShaderModule& ModuleHandle() const;
    [[nodiscard]] vk::ShaderModule& ModuleHandle();

private:
    vk::UniqueShaderModule m_shaderModule = {};
};
