#pragma once

#include "VulkanPipelineStateObject.h"
#include "VulkanDeviceResource.h"

class VulkanPipelineStateObjectResource : public VulkanDeviceResource
{
public:
    ~VulkanPipelineStateObjectResource() override = default;

protected:
    VulkanPipelineStateObjectResource(VulkanPipelineStateObject* pso) : VulkanDeviceResource(*pso), m_parentPso(pso)
    {}

    VulkanPipelineStateObjectResource(const VulkanPipelineStateObjectResource& other) = default;
    VulkanPipelineStateObjectResource& operator=(const VulkanPipelineStateObjectResource& other) = default;

    VulkanPipelineStateObjectResource(VulkanPipelineStateObjectResource&& other) = default;
    VulkanPipelineStateObjectResource& operator=(VulkanPipelineStateObjectResource&& other) = default;

    [[nodiscard]] const VulkanPipelineStateObject& ParentPipelineStateObject() const
    {
        return *m_parentPso;
    }

    [[nodiscard]] VulkanPipelineStateObject& ParentPipelineStateObject()
    {
        return *m_parentPso;
    }

private:
    VulkanPipelineStateObject* m_parentPso = nullptr;
};
