#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class PipelineResourceLayout : public DeviceResource
{
public:
    PipelineResourceLayout(Device * device);

    // TODO: Do this better
    [[nodiscard]] const vk::DescriptorSetLayout & DescriptorSetLayoutHandle() const;
    [[nodiscard]] vk::DescriptorSetLayout & DescriptorSetLayoutHandle();

private:
    vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
};

}  // namespace Gris::Graphics::Vulkan
