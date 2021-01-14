#pragma once

namespace Gris::Graphics::Glfw
{

class GlfwInstance
{
public:
    static void Init();

    static void PollEvents();

    GlfwInstance(const GlfwInstance &) = delete;
    GlfwInstance & operator=(const GlfwInstance &) = delete;

    GlfwInstance(GlfwInstance &&) = delete;
    GlfwInstance & operator=(GlfwInstance &&) = delete;

private:
    static GlfwInstance & Instance();

    GlfwInstance();
    ~GlfwInstance();
};

}  // namespace Gris::Graphics::Glfw
