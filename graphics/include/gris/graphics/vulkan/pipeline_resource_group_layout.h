#pragma once

#include <gris/graphics/vulkan/device_resource.h>

#include <gris/graphics/backend/pipeline_resource_layout.h>

#include <gris/span.h>

#include <map>

namespace Gris::Graphics::Vulkan
{

class PipelineResourceGroupLayout : public DeviceResource
{
public:
    PipelineResourceGroupLayout(Device * device, const Gris::Graphics::Backend::PipelineResourceGroupLayoutDesc & desc);

    // TODO: Do this better
    [[nodiscard]] const vk::DescriptorSetLayout & DescriptorSetLayoutHandle() const;
    [[nodiscard]] vk::DescriptorSetLayout & DescriptorSetLayoutHandle();

    const vk::DescriptorSetLayoutBinding & NameToBinding(const std::string_view & name) const;

private:
    vk::UniqueDescriptorSetLayout m_descriptorSetLayout = {};
    std::map<std::string, vk::DescriptorSetLayoutBinding, std::less<>> m_nameToBinding = {};
};

}  // namespace Gris::Graphics::Vulkan
