#pragma once

#include <gris/graphics/vulkan/buffer.h>
#include <gris/graphics/vulkan/buffer_view.h>
#include <gris/graphics/vulkan/deferred_context.h>
#include <gris/graphics/vulkan/device.h>
#include <gris/graphics/vulkan/framebuffer.h>
#include <gris/graphics/vulkan/glfw/window.h>
#include <gris/graphics/vulkan/pipeline_state_object.h>
#include <gris/graphics/vulkan/render_pass.h>
#include <gris/graphics/vulkan/sampler.h>
#include <gris/graphics/vulkan/shader.h>
#include <gris/graphics/vulkan/shader_resource_bindings.h>
#include <gris/graphics/vulkan/shader_resource_bindings_layout.h>
#include <gris/graphics/vulkan/shader_resource_bindings_pool_collection.h>
#include <gris/graphics/vulkan/swap_chain.h>
#include <gris/graphics/vulkan/texture.h>
#include <gris/graphics/vulkan/texture_view.h>

#include <gris/graphics/cameras/trackball_camera.h>
#include <gris/graphics/lens/perspective_lens.h>
#include <gris/graphics/mesh.h>

#include <cstdint>
#include <vector>

class ForwardRenderingApplication : public Gris::Graphics::WindowObserver
{
public:
    void Run();

private:
    constexpr static uint32_t GLOBAL_DESCRIPTOR_SET_INDEX = 0;
    constexpr static uint32_t PER_MATERIAL_DESCRIPTOR_SET_INDEX = 1;
    constexpr static uint32_t PER_DRAW_DESCRIPTOR_SET_INDEX = 2;
    constexpr static uint32_t DESCRIPTOR_SET_COUNT = 3;

    [[nodiscard]] vk::Format FindDepthFormat() const;

    void WindowResized(uint32_t width, uint32_t height) override;
    void MouseButtonEvent(Gris::Graphics::MouseButton button, Gris::Graphics::MouseButtonAction action, float x, float y) override;
    void MouseMoveEvent(float x, float y) override;
    void MouseWheelEvent(float x, float y, float delta) override;

    void InitWindow();
    void CreateVulkanObjects();
    void LoadScene();
    void MainLoop();

    void WaitForDeviceToBeIdle() const;

    void CreateDevice();
    void CreateSwapChain();
    void CreateRenderPass();
    void CreateCamera();
    void CreateMesh();
    void CreateMeshTexture();
    void CreatePipelineStateObject();
    void CreateFramebuffers();
    void CreateShaderResourceBindingsPools();
    void CreateUniformBuffersAndBindings();
    void CreateCommandBuffers();

    void UpdateUniformBuffer(uint32_t currentImage);
    void DrawFrame();

    Gris::Graphics::Vulkan::Glfw::Window m_window = {};
    Gris::Graphics::Vulkan::Device m_device = {};
    Gris::Graphics::Vulkan::SwapChain m_swapChain = {};

    std::vector<Gris::Graphics::Vulkan::Framebuffer> m_swapChainFramebuffers = {};

    Gris::Graphics::Vulkan::RenderPass m_renderPass = {};
    Gris::Graphics::Vulkan::Shader m_vertexShader = {};
    Gris::Graphics::Vulkan::Shader m_fragmentShader = {};

    std::array<Gris::Graphics::Vulkan::ShaderResourceBindingsLayout, DESCRIPTOR_SET_COUNT> m_resourceLayouts = {};
    Gris::Graphics::Vulkan::PipelineStateObject m_pso = {};

    Gris::Graphics::Backend::ShaderResourceBindingsPoolCategory m_shaderResourceBindingsPoolCategory = Gris::Graphics::Backend::ShaderResourceBindingsPoolCategory{ 0 };
    Gris::Graphics::Vulkan::ShaderResourceBindingsPoolCollection m_shaderResourceBindingsPools;

    std::vector<std::array<Gris::Graphics::Vulkan::ShaderResourceBindings, DESCRIPTOR_SET_COUNT>> m_shaderResourceBindings = {};

    Gris::Graphics::Vulkan::Texture m_colorImage = {};
    Gris::Graphics::Vulkan::TextureView m_colorImageView = {};

    Gris::Graphics::Vulkan::Texture m_depthImage = {};
    Gris::Graphics::Vulkan::TextureView m_depthImageView = {};

    std::vector<Gris::Graphics::Mesh> m_meshes;
    std::vector<Gris::Graphics::Vulkan::Buffer> m_vertexBuffers = {};
    std::vector<Gris::Graphics::Vulkan::BufferView> m_vertexBufferViews = {};
    std::vector<Gris::Graphics::Vulkan::Buffer> m_indexBuffers = {};
    std::vector<Gris::Graphics::Vulkan::BufferView> m_indexBufferViews = {};

    Gris::Graphics::Vulkan::Texture m_meshTextureImage = {};
    Gris::Graphics::Vulkan::TextureView m_meshTextureImageView = {};
    Gris::Graphics::Vulkan::Sampler m_meshTextureSampler = {};

    std::vector<Gris::Graphics::Vulkan::Buffer> m_uniformBuffers = {};
    std::vector<Gris::Graphics::Vulkan::BufferView> m_uniformBufferViews = {};

    std::vector<Gris::Graphics::Vulkan::DeferredContext> m_commandBuffers = {};

    Gris::Graphics::Cameras::TrackballCamera m_camera = {};
    Gris::Graphics::Lens::PerspectiveLens m_lens = {};

    bool m_framebufferResized = false;
};
