#include "Core/Controller.h"

#include <QApplication>
#include <QImageReader>

using namespace Canavar::Engine;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // qInstallMessageHandler(Logger::QtMessageOutputCallback);

    Controller controller;
    controller.Run();

    return app.exec();
}
