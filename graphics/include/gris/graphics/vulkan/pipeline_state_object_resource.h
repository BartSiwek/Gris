#pragma once

#include <gris/graphics/vulkan/device_resource.h>
#include <gris/graphics/vulkan/pipeline_state_object.h>

namespace Gris::Graphics::Vulkan
{

class VulkanPipelineStateObjectResource : public VulkanDeviceResource
{
public:
    ~VulkanPipelineStateObjectResource() override = default;

protected:
    VulkanPipelineStateObjectResource(VulkanPipelineStateObject * pso)
        : VulkanDeviceResource(*pso)
        , m_parentPso(pso)
    {
    }

    VulkanPipelineStateObjectResource(const VulkanPipelineStateObjectResource & other) = default;
    VulkanPipelineStateObjectResource & operator=(const VulkanPipelineStateObjectResource & other) = default;

    VulkanPipelineStateObjectResource(VulkanPipelineStateObjectResource && other) = default;
    VulkanPipelineStateObjectResource & operator=(VulkanPipelineStateObjectResource && other) = default;

    [[nodiscard]] const VulkanPipelineStateObject & ParentPipelineStateObject() const
    {
        return *m_parentPso;
    }

    [[nodiscard]] VulkanPipelineStateObject & ParentPipelineStateObject()
    {
        return *m_parentPso;
    }

private:
    VulkanPipelineStateObject * m_parentPso = nullptr;
};

}  // namespace Gris::Graphics::Vulkan
