#pragma once

#include <gris/span.h>

#include <cstdint>
#include <string_view>

namespace Gris::Graphics::Backend
{

enum class PipelineResourceType
{
    Sampler = 0,
    CombinedImageSampler = 1,
    SampledImage = 2,
    StorageImage = 3,
    UniformTexelBuffer = 4,
    StorageTexelBuffer = 5,
    UniformBuffer = 6,
    StorageBuffer = 7,
    UniformBufferDynamic = 8,
    StorageBufferDynamic = 9,
    InputAttachment = 10,
};

enum class PipelineStageFlags : uint32_t
{
    Vertex = 0x00000001,
    Hull = 0x00000002,
    Domain = 0x00000004,
    Geometry = 0x00000008,
    Fragment = 0x00000010,
    Compute = 0x00000020,
};

struct PipelineResourceLayoutDesc
{
    std::string_view Semantic = {};
    uint32_t Binding = 0;
    PipelineResourceType ResourceType = {};
    uint32_t Count = 0;
    PipelineStageFlags Stages = {};
};

struct PipelineResourceGroupLayoutDesc
{
    Gris::Span<const PipelineResourceLayoutDesc> ResourceLayouts;
};

}  // namespace Gris::Graphics::Backend
