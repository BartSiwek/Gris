#pragma once

#include <gris/graphics/vulkan/device_resource.h>

#include <gris/graphics/backend/shader_resource_bindings_layout.h>

#include <map>

namespace Gris::Graphics::Vulkan
{

class ShaderResourceBindingsLayout : public DeviceResource, public ParentObject<ShaderResourceBindingsLayout>
{
public:
    ShaderResourceBindingsLayout();

    ShaderResourceBindingsLayout(const ParentObject<Device> & device, const Gris::Graphics::Backend::ShaderResourceBindingsLayout & bindingsLayout);

    ShaderResourceBindingsLayout(const ShaderResourceBindingsLayout &) = delete;
    ShaderResourceBindingsLayout & operator=(const ShaderResourceBindingsLayout &) = delete;

    ShaderResourceBindingsLayout(ShaderResourceBindingsLayout && other) noexcept;
    ShaderResourceBindingsLayout & operator=(ShaderResourceBindingsLayout && other) noexcept;

    virtual ~ShaderResourceBindingsLayout();

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const vk::DescriptorSetLayout & DescriptorSetLayoutHandle() const;
    [[nodiscard]] vk::DescriptorSetLayout & DescriptorSetLayoutHandle();

    const vk::DescriptorSetLayoutBinding & NameToBinding(const std::string_view & name) const;

    void Reset();

private:
    struct NameAndBinding
    {
        std::string Name;
        vk::DescriptorSetLayoutBinding Binding;
    };

    vk::DescriptorSetLayout m_descriptorSetLayout = {};
    std::vector<NameAndBinding> m_nameToBinding = {};
};

}  // namespace Gris::Graphics::Vulkan
