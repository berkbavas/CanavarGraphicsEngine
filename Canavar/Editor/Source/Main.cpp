#include "Core/Controller.h"
#include "Node/Model/Model.h"

#include <QApplication>
#include <QImageReader>

using namespace Canavar::Engine;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Controller controller;
    controller.Run();

    return app.exec();
}
