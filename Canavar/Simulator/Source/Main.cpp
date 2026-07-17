#include "Simulator.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int Argc, char* Argv[])
{
    QApplication Application(Argc, Argv);

    Canavar::Simulator::Simulator Simulator;
    Simulator.Run();

    return Application.exec();
}
