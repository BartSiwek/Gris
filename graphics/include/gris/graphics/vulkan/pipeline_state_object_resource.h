#pragma once

#include <gris/graphics/vulkan/device_resource.h>
#include <gris/graphics/vulkan/pipeline_state_object.h>

namespace Gris::Graphics::Vulkan
{

class PipelineStateObjectResource : public DeviceResource
{
public:
    ~PipelineStateObjectResource() override = default;

protected:
    PipelineStateObjectResource(PipelineStateObject * pso)
        : DeviceResource(*pso)
        , m_parentPso(pso)
    {
    }

    PipelineStateObjectResource(const PipelineStateObjectResource & other) = default;
    PipelineStateObjectResource & operator=(const PipelineStateObjectResource & other) = default;

    PipelineStateObjectResource(PipelineStateObjectResource && other) = default;
    PipelineStateObjectResource & operator=(PipelineStateObjectResource && other) = default;

    [[nodiscard]] const PipelineStateObject & ParentPipelineStateObject() const
    {
        return *m_parentPso;
    }

    [[nodiscard]] PipelineStateObject & ParentPipelineStateObject()
    {
        return *m_parentPso;
    }

private:
    PipelineStateObject * m_parentPso = nullptr;
};

}  // namespace Gris::Graphics::Vulkan
