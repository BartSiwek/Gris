#pragma once

#include <gris/graphics/glfw/window_mixin.h>
#include <gris/graphics/vulkan/window_mixin.h>

namespace Gris::Graphics::Vulkan::Glfw
{

class Window : public Graphics::Glfw::WindowMixin, public Graphics::Vulkan::WindowMixin
{
public:
    Window();

    Window(uint32_t width, uint32_t height, const std::string & title);

    Window(const Window &) = delete;
    Window & operator=(const Window &) = delete;

    Window(Window &&) noexcept = default;
    Window & operator=(Window &&) noexcept = default;

    ~Window() override = default;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;
};

}  // namespace Gris::Graphics::Vulkan::Glfw
