#pragma once

#include <gris/strong_type.h>

#include <cstdint>

namespace Gris::Graphics::Backend
{

/*
 * This factors are not relevant for OpenGL and DirectX 11.
 * For DX12 this might need re-working.
 */
struct ShaderResourceBindingsPoolSizes
{
    constexpr static size_t FactorCount = 11;

    uint32_t ShaderResourceBindingsCount = 0;

    uint32_t SamplerCount = 0;
    uint32_t CombinedImageSamplerCount = 0;
    uint32_t SampledImageCount = 0;
    uint32_t StorageImageCount = 0;
    uint32_t UniformTexelBufferCount = 0;
    uint32_t StorageTexelBufferCount = 0;
    uint32_t UniformBufferCount = 0;
    uint32_t StorageBufferCount = 0;
    uint32_t DynamicUniformBufferCount = 0;
    uint32_t DynamicStorageBufferCount = 0;
    uint32_t InputAttachmentCount = 0;
};


using ShaderResourceBindingsPoolCategory = StrongType<
    uint32_t,
    struct ShaderResourceBindingsPoolCategoryTag,
    std::numeric_limits<uint32_t>::max(),
    StrongTypeMixins::Comparable,
    StrongTypeMixins::Hashable,
    StrongTypeMixins::HasInvalidValue<std::numeric_limits<uint32_t>::max()>>;

}  // namespace Gris::Graphics::Backend
