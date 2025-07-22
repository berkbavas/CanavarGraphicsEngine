#include "Viewer.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Canavar::Viewer::Viewer viewer;
    viewer.Run();

    return app.exec();
}
