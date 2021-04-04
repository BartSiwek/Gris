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

    WindowMixin(const WindowMixin & other) = delete;
    WindowMixin & operator=(const WindowMixin & other) = delete;

    WindowMixin(WindowMixin && other) noexcept;
    WindowMixin & operator=(WindowMixin && other) noexcept;

    virtual ~WindowMixin();

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

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
    void OnMouseButtonEvent(MouseButton button, MouseButtonAction action, float x, float y);
    void OnMouseMoveEvent(float x, float y);
    void OnMouseWheelEvent(float x, float y, float delta);

    void NotifySizeChanged();
    void NotifyMouseButtonEvent(MouseButton button, MouseButtonAction action, float x, float y);
    void NotifyMouseMoveEvent(float x, float y);
    void NotifyMouseWheelEvent(float x, float y, float delta);

    uint32_t m_width = 0;
    uint32_t m_height = 0;
    GLFWwindow * m_window = nullptr;
    std::vector<WindowObserver *> m_observers = {};
};

}  // namespace Gris::Graphics::Glfw
