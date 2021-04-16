#pragma once

#include <gris/enum.h>
#include <gris/span.h>

#include <cstdint>

namespace Gris::Graphics::Backend
{

enum class ShaderResourceType
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

enum class ShaderStageFlags : uint32_t
{
    Vertex = 0x00000001,
    Hull = 0x00000002,
    Domain = 0x00000004,
    Geometry = 0x00000008,
    Fragment = 0x00000010,
    Compute = 0x00000020,
};

GRIS_ENABLE_ENUM_BIT_FLAGS_OPERATIONS(ShaderStageFlags)

struct ShaderResourceBindingLayout
{
    std::string_view Semantic = {};
    uint32_t Binding = 0;
    ShaderResourceType Type = {};
    uint32_t Count = 0;
    ShaderStageFlags Stages = {};
};

struct ShaderResourceBindingsLayout
{
    Gris::Span<const ShaderResourceBindingLayout> Layouts;
};

}  // namespace Gris::Graphics::Backend
