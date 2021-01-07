#include <gris/graphics/vulkan/VulkanImmediateContext.h>

#include <gris/graphics/vulkan/VulkanBuffer.h>
#include <gris/graphics/vulkan/VulkanDeferredContext.h>
#include <gris/graphics/vulkan/VulkanDevice.h>
#include <gris/graphics/vulkan/VulkanEngineException.h>
#include <gris/graphics/vulkan/VulkanFence.h>
#include <gris/graphics/vulkan/VulkanSemaphore.h>
#include <gris/graphics/vulkan/VulkanTexture.h>

#include <iterator>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanImmediateContext::VulkanImmediateContext(VulkanDevice * device)
    : VulkanDeviceResource(device)
{
    auto const queueFamilies = ParentDevice().QueueFamilies();
    auto const graphicsQueueFamily = queueFamilies.graphicsFamily.value();

    m_graphicsQueue = DeviceHandle().getQueue(graphicsQueueFamily, 0);

    auto const poolInfo = vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eTransient, graphicsQueueFamily);

    auto createCommandPoolResult = DeviceHandle().createCommandPoolUnique(poolInfo);
    if (createCommandPoolResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating command pool", createCommandPoolResult);

    m_commandPool = std::move(createCommandPoolResult.value);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanImmediateContext::GenerateMipmaps(const VulkanTexture & texture, const vk::Format & imageFormat, int32_t texWidth, int32_t texHeight)
{
    auto const formatProperties = ParentDevice().GetFormatProperties(imageFormat);

    if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
        throw VulkanEngineException("Texture image format does not support linear blitting!");

    auto commandBuffer = BeginSingleTimeCommands();

    std::array barriers = {
        vk::ImageMemoryBarrier({},
                               {},
                               {},
                               {},
                               VK_QUEUE_FAMILY_IGNORED,
                               VK_QUEUE_FAMILY_IGNORED,
                               texture.ImageHandle(),
                               vk::ImageSubresourceRange(
                                   vk::ImageAspectFlagBits::eColor,
                                   0,
                                   1,
                                   0,
                                   1))
    };

    auto mipWidth = texWidth;
    auto mipHeight = texHeight;

    for (uint32_t i = 1; i < texture.MipLevels(); i++)
    {
        barriers[0].subresourceRange.baseMipLevel = i - 1;
        barriers[0].oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barriers[0].newLayout = vk::ImageLayout::eTransferSrcOptimal;
        barriers[0].srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barriers[0].dstAccessMask = vk::AccessFlagBits::eTransferRead;

        commandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, barriers);

        auto const blit = vk::ImageBlit(
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor,
                                       i - 1,
                                       0,
                                       1),
            std::array{ vk::Offset3D(0, 0, 0), vk::Offset3D(mipWidth, mipHeight, 1) },
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor,
                                       i,
                                       0,
                                       1),
            std::array{ vk::Offset3D(0, 0, 0), vk::Offset3D(mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1) });

        std::array blits = { blit };
        commandBuffer->blitImage(texture.ImageHandle(), vk::ImageLayout::eTransferSrcOptimal, texture.ImageHandle(), vk::ImageLayout::eTransferDstOptimal, blits, {});

        barriers[0].oldLayout = vk::ImageLayout::eTransferSrcOptimal;
        barriers[0].newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        barriers[0].srcAccessMask = vk::AccessFlagBits::eTransferRead;
        barriers[0].dstAccessMask = vk::AccessFlagBits::eShaderRead;

        commandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, barriers);

        if (mipWidth > 1)
            mipWidth /= 2;
        if (mipHeight > 1)
            mipHeight /= 2;
    }

    barriers[0].subresourceRange.baseMipLevel = texture.MipLevels() - 1;
    barriers[0].oldLayout = vk::ImageLayout::eTransferDstOptimal;
    barriers[0].newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    barriers[0].srcAccessMask = vk::AccessFlagBits::eTransferRead;
    barriers[0].dstAccessMask = vk::AccessFlagBits::eShaderRead;

    commandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, barriers);

    EndSingleTimeCommands(commandBuffer.get());
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanImmediateContext::CopyBufferToImage(const VulkanBuffer & buffer, const VulkanTexture & texture, uint32_t width, uint32_t height)
{
    auto commandBuffer = BeginSingleTimeCommands();

    auto const region = vk::BufferImageCopy(0,
                                            0,
                                            0,
                                            vk::ImageSubresourceLayers(
                                                vk::ImageAspectFlagBits::eColor,
                                                0,
                                                0,
                                                1),
                                            vk::Offset3D(0, 0, 0),
                                            vk::Extent3D(width, height, 1));

    std::array regions = { region };

    commandBuffer->copyBufferToImage(buffer.BufferHandle(), texture.ImageHandle(), vk::ImageLayout::eTransferDstOptimal, regions);

    EndSingleTimeCommands(commandBuffer.get());
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanImmediateContext::TransitionImageLayout(const VulkanTexture & texture, const vk::ImageLayout & oldLayout, const vk::ImageLayout & newLayout)
{
    auto commandBuffer = BeginSingleTimeCommands();

    vk::ImageMemoryBarrier barrier({},
                                   {},
                                   oldLayout,
                                   newLayout,
                                   VK_QUEUE_FAMILY_IGNORED,
                                   VK_QUEUE_FAMILY_IGNORED,
                                   texture.ImageHandle(),
                                   vk::ImageSubresourceRange(
                                       vk::ImageAspectFlagBits::eColor,
                                       0,
                                       texture.MipLevels(),
                                       0,
                                       1));

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else
    {
        throw VulkanEngineException("Unsupported layout transition!");
    }

    std::array imageMemoryBarriers = { barrier };
    commandBuffer->pipelineBarrier(sourceStage, destinationStage, {}, {}, {}, imageMemoryBarriers);

    EndSingleTimeCommands(commandBuffer.get());
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanImmediateContext::CopyBuffer(const VulkanBuffer & srcBuffer, const VulkanBuffer & dstBuffer, vk::DeviceSize size)
{
    auto commandBuffer = BeginSingleTimeCommands();

    vk::BufferCopy copyRegion(0, 0, size);
    commandBuffer->copyBuffer(srcBuffer.BufferHandle(), dstBuffer.BufferHandle(), 1, &copyRegion);

    EndSingleTimeCommands(commandBuffer.get());
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanImmediateContext::Submit(
    VulkanDeferredContext * context,
    const std::vector<std::reference_wrapper<VulkanSemaphore>> & waitSemaphores,
    const std::vector<std::reference_wrapper<VulkanSemaphore>> & signalSemaphores,
    VulkanFence & fence)
{
    auto * deferredContext = static_cast<VulkanDeferredContext *>(context);

    std::vector<vk::Semaphore> vulkanWaitSemaphores;
    std::transform(waitSemaphores.begin(), waitSemaphores.end(), std::back_inserter(vulkanWaitSemaphores), [](const auto & semaphore)
                   { return semaphore.get().SemaphoreHandle(); });

    std::vector<vk::Semaphore> vulkanSignalSemaphores;
    std::transform(signalSemaphores.begin(), signalSemaphores.end(), std::back_inserter(vulkanSignalSemaphores), [](const auto & semaphore)
                   { return semaphore.get().SemaphoreHandle(); });

    std::array waitStages = { vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput) };
    std::array commandBuffers = { deferredContext->CommandBufferHandle() };
    std::array submits = { vk::SubmitInfo(vulkanWaitSemaphores, waitStages, commandBuffers, vulkanSignalSemaphores) };

    auto const submitResult = m_graphicsQueue.submit(submits, fence.FenceHandle());
    if (submitResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error submitting to graphics queue", submitResult);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::UniqueCommandBuffer Gris::Graphics::Vulkan::VulkanImmediateContext::BeginSingleTimeCommands()
{
    auto const allocInfo = vk::CommandBufferAllocateInfo(
        m_commandPool.get(),
        vk::CommandBufferLevel::ePrimary,
        1);

    auto allocateCommandBuffersResult = DeviceHandle().allocateCommandBuffersUnique(allocInfo);
    if (allocateCommandBuffersResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error allocating command buffers", allocateCommandBuffersResult);

    auto && commandBuffer = allocateCommandBuffersResult.value.front();

    auto const beginInfo = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    auto const beginResult = commandBuffer->begin(beginInfo);
    if (beginResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error beginning the command buffer", beginResult);

    return std::move(commandBuffer);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanImmediateContext::EndSingleTimeCommands(vk::CommandBuffer & commandBuffer) const
{
    auto const endResult = commandBuffer.end();
    if (endResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error ending the command buffer", endResult);

    std::array commandBuffers = { commandBuffer };
    std::array submits = { vk::SubmitInfo({}, {}, commandBuffers, {}) };

    auto const submitResult = m_graphicsQueue.submit(submits, vk::Fence());
    if (submitResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error submitting the command buffer", submitResult);

    // TODO: There has to be a better way to do this
    auto const waitResult = m_graphicsQueue.waitIdle();
    if (waitResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error waiting for graphics queue to be idle", waitResult);
}
