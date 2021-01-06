#include <gris/graphics/vulkan/VulkanRenderPass.h>

#include <gris/graphics/vulkan/VulkanDevice.h>
#include <gris/graphics/vulkan/VulkanEngineException.h>

// -------------------------------------------------------------------------------------------------

VulkanRenderPass::VulkanRenderPass(VulkanDevice* device, vk::Format swapChainFormat, vk::Format depthFormat) : VulkanDeviceResource(device)
{
    CreateRenderPass(swapChainFormat, depthFormat);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::RenderPass& VulkanRenderPass::RenderPassHandle() const
{
    return m_renderPass.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::RenderPass& VulkanRenderPass::RenderPassHandle()
{
    return m_renderPass.get();
}

// -------------------------------------------------------------------------------------------------

void VulkanRenderPass::CreateRenderPass(vk::Format swapChainFormat, vk::Format depthFormat) {
    auto const colorAttachment = vk::AttachmentDescription(
        {},
        swapChainFormat,
        ParentDevice().MsaaSamples(),
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal
    );

    auto const depthAttachment = vk::AttachmentDescription(
        {},
        depthFormat,
        ParentDevice().MsaaSamples(),
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eDontCare,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthStencilAttachmentOptimal
    );

    auto const colorAttachmentResolve = vk::AttachmentDescription(
        {},
        swapChainFormat,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR
    );

    auto const colorAttachmentRef = vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);
    auto const depthAttachmentRef = vk::AttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    auto const colorAttachmentResolveRef = vk::AttachmentReference(2, vk::ImageLayout::eColorAttachmentOptimal);

    auto const attachments = std::array{ colorAttachment, depthAttachment, colorAttachmentResolve };

    auto const subpasses = std::array{
        vk::SubpassDescription(
            {},
            vk::PipelineBindPoint::eGraphics,
            0,
            nullptr,
            1,
            &colorAttachmentRef,
            &colorAttachmentResolveRef,
            &depthAttachmentRef,
            0,
            nullptr
        )
    };

    auto const dependencies = std::array{
        vk::SubpassDependency(
            VK_SUBPASS_EXTERNAL,
            0,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            {},
            vk::AccessFlagBits::eColorAttachmentWrite
        )
    };

    auto const renderPassInfo = vk::RenderPassCreateInfo({}, attachments, subpasses, dependencies);

    auto createRenderPassResult = DeviceHandle().createRenderPassUnique(renderPassInfo);
    if (createRenderPassResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating render pass", createRenderPassResult);

    m_renderPass = std::move(createRenderPassResult.value);
}
