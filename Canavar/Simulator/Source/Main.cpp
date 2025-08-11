#include "Simulator.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char* argv[])
{
    QSurfaceFormat format;
    format.setVersion(4, 5);                              // Request OpenGL version 4.5
    format.setProfile(QSurfaceFormat::CoreProfile);       // Request Core Profile
    format.setDepthBufferSize(24);                        // 24-bit depth buffer
    format.setStencilBufferSize(8);                       // 8-bit stencil buffer
    format.setSamples(0);                                 // No multisampling
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer); // Double buffering (default)
    format.setSwapInterval(1);                            // Enable vsync (1 frame per screen refresh)
    format.setOption(QSurfaceFormat::DebugContext);       // Request debug context (helps debugging)

    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);

    Canavar::Simulator::Simulator simulator;
    simulator.Run();

    return app.exec();
}
