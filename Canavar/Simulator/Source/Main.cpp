#include "Simulator.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Canavar::Simulator::Simulator *pSimulator = new Canavar::Simulator::Simulator();
    pSimulator->Run();

    return app.exec();
}
