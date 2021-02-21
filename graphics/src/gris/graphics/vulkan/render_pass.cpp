#include <gris/graphics/vulkan/render_pass.h>

#include <gris/graphics/vulkan/device.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::RenderPass::RenderPass() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::RenderPass::RenderPass(const ParentObject<Device> & device, vk::Format swapChainFormat, vk::Format depthFormat)
    : DeviceResource(device)
{
    auto const colorAttachment = vk::AttachmentDescription{}
                                     .setFormat(swapChainFormat)
                                     .setSamples(ParentDevice().MsaaSamples())
                                     .setLoadOp(vk::AttachmentLoadOp::eClear)
                                     .setStoreOp(vk::AttachmentStoreOp::eStore)
                                     .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                                     .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                                     .setInitialLayout(vk::ImageLayout::eUndefined)
                                     .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

    auto const depthAttachment = vk::AttachmentDescription{}
                                     .setFormat(depthFormat)
                                     .setSamples(ParentDevice().MsaaSamples())
                                     .setLoadOp(vk::AttachmentLoadOp::eClear)
                                     .setStoreOp(vk::AttachmentStoreOp::eDontCare)
                                     .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                                     .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                                     .setInitialLayout(vk::ImageLayout::eUndefined)
                                     .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    auto const colorAttachmentResolve = vk::AttachmentDescription{}
                                            .setFormat(swapChainFormat)
                                            .setSamples(vk::SampleCountFlagBits::e1)
                                            .setLoadOp(vk::AttachmentLoadOp::eDontCare)
                                            .setStoreOp(vk::AttachmentStoreOp::eStore)
                                            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                                            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                                            .setInitialLayout(vk::ImageLayout::eUndefined)
                                            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    auto const colorAttachmentRef = vk::AttachmentReference{}.setAttachment(0).setLayout(vk::ImageLayout::eColorAttachmentOptimal);
    auto const depthAttachmentRef = vk::AttachmentReference{}.setAttachment(1).setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
    auto const colorAttachmentResolveRef = vk::AttachmentReference{}.setAttachment(2).setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    auto const attachments = std::array{ colorAttachment, depthAttachment, colorAttachmentResolve };

    const auto colorAttachments = std::array{ colorAttachmentRef };
    const auto resolveAttachments = std::array{ colorAttachmentResolveRef };

    auto const subpasses = std::array{
        vk::SubpassDescription{}
            .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
            .setColorAttachments(colorAttachments)
            .setResolveAttachments(resolveAttachments)
            .setPDepthStencilAttachment(&depthAttachmentRef)
    };

    auto const dependencies = std::array{
        vk::SubpassDependency{}
            .setSrcSubpass(VK_SUBPASS_EXTERNAL)
            .setDstSubpass(0)
            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setSrcAccessMask({})
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
    };

    auto const renderPassInfo = vk::RenderPassCreateInfo{}
                                    .setAttachments(attachments)
                                    .setSubpasses(subpasses)
                                    .setDependencies(dependencies);

    auto createRenderPassResult = DeviceHandle().createRenderPass(renderPassInfo, nullptr, Dispatch());
    if (createRenderPassResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating render pass", createRenderPassResult);
    }

    m_renderPass = std::move(createRenderPassResult.value);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::RenderPass::RenderPass(RenderPass && other) noexcept
    : DeviceResource(std::move(other))
    , m_renderPass(std::exchange(other.m_renderPass, {}))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::RenderPass & Gris::Graphics::Vulkan::RenderPass::operator=(RenderPass && other) noexcept
{
    if (this != &other)
    {
        ReleaseResources();

        DeviceResource::operator=(std::move(other));
        m_renderPass = std::exchange(other.m_renderPass, {});
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::RenderPass::~RenderPass()
{
    ReleaseResources();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::RenderPass::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::RenderPass::IsValid() const
{
    return DeviceResource::IsValid() && static_cast<bool>(m_renderPass);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::RenderPass & Gris::Graphics::Vulkan::RenderPass::RenderPassHandle() const
{
    return m_renderPass;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::RenderPass & Gris::Graphics::Vulkan::RenderPass::RenderPassHandle()
{
    return m_renderPass;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::RenderPass::Reset()
{
    ReleaseResources();
    ResetParent();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::RenderPass::ReleaseResources()
{
    if (m_renderPass)
    {
        DeviceHandle().destroyRenderPass(m_renderPass, nullptr, Dispatch());
        m_renderPass = nullptr;
    }
}
