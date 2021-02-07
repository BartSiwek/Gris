#pragma once

#include <gris/graphics/window_observer.h>

#include <string>
#include <vector>

struct GLFWwindow;

namespace Gris::Graphics::Glfw
{

class WindowMixin
{
public:
    WindowMixin() = default;

    WindowMixin(uint32_t width, uint32_t height, const std::string & title);
    virtual ~WindowMixin();

    WindowMixin(const WindowMixin & other) = delete;
    WindowMixin & operator=(const WindowMixin & other) = delete;

    WindowMixin(WindowMixin && other) noexcept;
    WindowMixin & operator=(WindowMixin && other) noexcept;

    explicit operator bool() const;

    bool IsValid() const;

    [[nodiscard]] uint32_t Width() const;
    [[nodiscard]] uint32_t Height() const;

    [[nodiscard]] bool ShouldClose() const;

    void AddObserver(WindowObserver * observer);
    void RemoveObserver(WindowObserver * observer);

protected:
    [[nodiscard]] const GLFWwindow * WindowHandle() const;
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
