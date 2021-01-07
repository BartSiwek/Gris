#include <gris/graphics/vulkan/VulkanDeferredContext.h>


#include <gris/graphics/vulkan/VulkanBufferView.h>
#include <gris/graphics/vulkan/VulkanDevice.h>
#include <gris/graphics/vulkan/VulkanEngineException.h>
#include <gris/graphics/vulkan/VulkanFramebuffer.h>
#include <gris/graphics/vulkan/VulkanPipelineStateObject.h>
#include <gris/graphics/vulkan/VulkanRenderPass.h>
#include <gris/graphics/vulkan/VulkanShaderResourceBinding.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanDeferredContext::VulkanDeferredContext(VulkanDevice * device)
    : VulkanDeviceResource(device)
{
    auto const queueFamilies = ParentDevice().QueueFamilies();
    auto const graphicsQueueFamily = queueFamilies.graphicsFamily.value();

    auto const poolInfo = vk::CommandPoolCreateInfo({}, graphicsQueueFamily);

    auto createCommandPoolResult = DeviceHandle().createCommandPoolUnique(poolInfo);
    if (createCommandPoolResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating command pool", createCommandPoolResult);

    m_commandPool = std::move(createCommandPoolResult.value);

    auto const allocInfo = vk::CommandBufferAllocateInfo(m_commandPool.get(), vk::CommandBufferLevel::ePrimary, 1);
    auto allocateCommandBuffersResult = DeviceHandle().allocateCommandBuffersUnique(allocInfo);
    if (allocateCommandBuffersResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error allocating command buffers", allocateCommandBuffersResult);

    m_commandBuffers = std::move(allocateCommandBuffersResult.value);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::CommandBuffer & Gris::Graphics::Vulkan::VulkanDeferredContext::CommandBufferHandle()
{
    return m_commandBuffers[m_frameIndex].get();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanDeferredContext::Begin()
{
    auto const beginInfo = vk::CommandBufferBeginInfo();

    auto const beginResult = m_commandBuffers[m_frameIndex]->begin(beginInfo);
    if (beginResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error beginning command buffer", beginResult);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanDeferredContext::BeginRenderPass(const VulkanRenderPass & renderPass, const VulkanFramebuffer & framebuffer, const vk::Extent2D & extent)
{
    std::array<vk::ClearValue, 2> clearValues = {
        vk::ClearColorValue(std::array{ 0.0f, 0.0f, 0.0f, 1.0f }),
        vk::ClearDepthStencilValue(1.0f, 0)
    };

    auto const renderPassInfo = vk::RenderPassBeginInfo(renderPass.RenderPassHandle(), framebuffer.FramebufferHandle(), vk::Rect2D({ 0, 0 }, extent), clearValues);

    m_commandBuffers[m_frameIndex]->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanDeferredContext::BindPipeline(const VulkanPipelineStateObject & pso)
{
    m_commandBuffers[m_frameIndex]->bindPipeline(vk::PipelineBindPoint::eGraphics, pso.GraphicsPipelineHandle());
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanDeferredContext::BindVertexBuffer(const VulkanBufferView & bufferView)
{
    std::array vertexBuffers = { bufferView.BufferHandle() };
    std::array offsets = { static_cast<vk::DeviceSize>(bufferView.Offset()) };
    m_commandBuffers[m_frameIndex]->bindVertexBuffers(0, vertexBuffers, offsets);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanDeferredContext::BindIndexBuffer(const VulkanBufferView & bufferView)
{
    m_commandBuffers[m_frameIndex]->bindIndexBuffer(bufferView.BufferHandle(), bufferView.Offset(), vk::IndexType::eUint32);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanDeferredContext::BindDescriptorSet(const VulkanPipelineStateObject & pso, const VulkanShaderResourceBinding & srb)
{
    std::array descriptorSets = { srb.DescriptorSetHandle(m_frameIndex) };
    m_commandBuffers[m_frameIndex]->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pso.PipelineLayoutHandle(), 0, descriptorSets, {});
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanDeferredContext::DrawIndexed(uint32_t indexCount)
{
    m_commandBuffers[m_frameIndex]->drawIndexed(indexCount, 1, 0, 0, 0);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanDeferredContext::EndRenderPass()
{
    m_commandBuffers[m_frameIndex]->endRenderPass();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanDeferredContext::End()
{
    auto const endResult = m_commandBuffers[m_frameIndex]->end();
    if (endResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error ending command buffer", endResult);
}
