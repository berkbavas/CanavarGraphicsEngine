#include "BuilderWindow.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int Argc, char* Argv[])
{
    QApplication App(Argc, Argv);
    App.setApplicationName("Terrain Builder");
    App.setOrganizationName("CanavarGraphicsEngine");

    QSurfaceFormat Fmt;
    Fmt.setDepthBufferSize(24);
    Fmt.setVersion(3, 3);
    Fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(Fmt);

    Canavar::Globe::BuilderWindow Window;
    Window.setWindowTitle("Terrain Builder  –  CanavarGraphicsEngine");
    Window.show();

    return App.exec();
}
