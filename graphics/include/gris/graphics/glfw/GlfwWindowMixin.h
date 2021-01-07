#pragma once

#include <gris/graphics/WindowObserver.h>

#include <string>
#include <vector>

struct GLFWwindow;

namespace Gris::Graphics::Glfw
{

class GlfwWindowMixin
{
public:
    GlfwWindowMixin(uint32_t width, uint32_t height, const std::string & title);
    virtual ~GlfwWindowMixin();

    GlfwWindowMixin(const GlfwWindowMixin & other) = delete;
    GlfwWindowMixin & operator=(const GlfwWindowMixin & other) = delete;

    GlfwWindowMixin(GlfwWindowMixin && other) noexcept;
    GlfwWindowMixin & operator=(GlfwWindowMixin && other) noexcept;

    explicit operator bool() const;

    [[nodiscard]] uint32_t Width() const;
    [[nodiscard]] uint32_t Height() const;

    [[nodiscard]] bool ShouldClose() const;

    void AddObserver(WindowObserver * observer);
    void RemoveObserver(WindowObserver * observer);

protected:
    [[nodiscard]] const GLFWwindow * const WindowHandle() const;
    [[nodiscard]] GLFWwindow * WindowHandle();

private:
    void OnSizeChanged(uint32_t width, uint32_t height);
    void NotifySizeChanged();

    uint32_t m_width = 0;
    uint32_t m_height = 0;
    GLFWwindow * m_window = nullptr;
    std::vector<WindowObserver *> m_observers = {};
};

}  // namespace Gris::Graphics::Glfw
