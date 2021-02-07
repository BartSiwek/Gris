#pragma once

#include <gris/graphics/vulkan/device_resource.h>

#include <gris/graphics/backend/shader_resource_bindings_layout.h>

#include <map>

namespace Gris::Graphics::Vulkan
{

class ShaderResourceBindingsLayout : public DeviceResource
{
public:
    ShaderResourceBindingsLayout(Device * device, const Gris::Graphics::Backend::ShaderResourceBindingsLayout & bindingsLayout);

    [[nodiscard]] const vk::DescriptorSetLayout & DescriptorSetLayoutHandle() const;
    [[nodiscard]] vk::DescriptorSetLayout & DescriptorSetLayoutHandle();

    const vk::DescriptorSetLayoutBinding & NameToBinding(const std::string_view & name) const;

private:
    struct NameAndBinding
    {
        std::string Name;
        vk::DescriptorSetLayoutBinding Binding;
    };

    vk::UniqueDescriptorSetLayout m_descriptorSetLayout = {};
    std::vector<NameAndBinding> m_nameToBinding = {};
};

}  // namespace Gris::Graphics::Vulkan
