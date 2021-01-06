#include <gris/graphics/glfw/GlfwInstance.h>

#include <GLFW/glfw3.h>

// -------------------------------------------------------------------------------------------------

void GlfwInstance::Init()
{
    Instance();
}

// -------------------------------------------------------------------------------------------------

void GlfwInstance::PollEvents()
{
    Instance();
    glfwPollEvents();
}

// -------------------------------------------------------------------------------------------------

GlfwInstance& GlfwInstance::Instance()
{
    static GlfwInstance s_instance = {};
    return s_instance;
}

// -------------------------------------------------------------------------------------------------

GlfwInstance::GlfwInstance()
{
    glfwInit();
}

// -------------------------------------------------------------------------------------------------

GlfwInstance::~GlfwInstance()
{
    glfwTerminate();
}
