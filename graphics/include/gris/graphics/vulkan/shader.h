#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Shader : public DeviceResource
{
public:
    Shader();

    Shader(std::shared_ptr<Device *> device, const std::vector<uint32_t> & code, std::string entryPoint);

    Shader(const Shader &) = delete;
    Shader & operator=(const Shader &) = delete;

    Shader(Shader &&) noexcept = default;
    Shader & operator=(Shader &&) noexcept = default;

    ~Shader() = default;

    explicit operator bool() const;

    bool IsValid() const;

    [[nodiscard]] const vk::ShaderModule & ModuleHandle() const;
    [[nodiscard]] vk::ShaderModule & ModuleHandle();

    [[nodiscard]] const std::string & EntryPoint() const;

private:
    vk::UniqueShaderModule m_shaderModule = {};
    std::string m_entryPoint = {};
};

}  // namespace Gris::Graphics::Vulkan
