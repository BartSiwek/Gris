#include "forward_rendering_application.h"

#include <gris/log.h>

int main()
{
    SetLogLevel(Gris::Log::Level::Trace);

    try
    {
        ForwardRenderingApplication app;
        app.Run();
    }
    catch (const std::exception & e)
    {
        Gris::Log::Critical(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
