﻿#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Shader : public DeviceResource
{
public:
    Shader(Device * device, const std::vector<char> & code);

    // TODO: Do this better
    [[nodiscard]] const vk::ShaderModule & ModuleHandle() const;
    [[nodiscard]] vk::ShaderModule & ModuleHandle();

private:
    vk::UniqueShaderModule m_shaderModule = {};
};

}  // namespace Gris::Graphics::Vulkan
