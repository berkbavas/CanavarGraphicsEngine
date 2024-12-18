#include "Simulator.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    // QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);

    Canavar::Simulator::Simulator simulator;
    simulator.Run();

    return app.exec();
}
