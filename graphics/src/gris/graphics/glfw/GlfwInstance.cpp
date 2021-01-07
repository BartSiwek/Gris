#include <gris/graphics/glfw/GlfwInstance.h>

#include <GLFW/glfw3.h>

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::GlfwInstance::Init()
{
    Instance();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::GlfwInstance::PollEvents()
{
    Instance();
    glfwPollEvents();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::GlfwInstance & Gris::Graphics::Glfw::GlfwInstance::Instance()
{
    static GlfwInstance s_instance = {};
    return s_instance;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::GlfwInstance::GlfwInstance()
{
    glfwInit();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::GlfwInstance::~GlfwInstance()
{
    glfwTerminate();
}
