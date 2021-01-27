#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Shader : public DeviceResource
{
public:
    Shader(Device * device, const std::vector<uint32_t> & code, std::string entryPoint);

    // TODO: Do this better
    [[nodiscard]] const vk::ShaderModule & ModuleHandle() const;
    [[nodiscard]] vk::ShaderModule & ModuleHandle();

    [[nodiscard]] const std::string & EntryPoint() const;

private:
    vk::UniqueShaderModule m_shaderModule = {};
    std::string m_entryPoint = {};
};

}  // namespace Gris::Graphics::Vulkan
