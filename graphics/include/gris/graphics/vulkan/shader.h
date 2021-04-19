#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Shader : public DeviceResource
{
public:
    Shader();

    Shader(const ParentObject<Device> & device, const std::vector<uint32_t> & code, std::string entryPoint);

    Shader(const Shader &) = delete;
    Shader & operator=(const Shader &) = delete;

    Shader(Shader && other) noexcept;
    Shader & operator=(Shader && other) noexcept;

    ~Shader() override;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const vk::ShaderModule & ModuleHandle() const;
    [[nodiscard]] vk::ShaderModule & ModuleHandle();

    [[nodiscard]] const std::string & EntryPoint() const;

    void Reset();

private:
    void ReleaseResources();

    vk::ShaderModule m_shaderModule = {};
    std::string m_entryPoint = {};
};

}  // namespace Gris::Graphics::Vulkan
