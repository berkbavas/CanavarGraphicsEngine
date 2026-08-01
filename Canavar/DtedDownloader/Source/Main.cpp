#include "DtedWindow.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("GLO-30 DEM Downloader");
    app.setOrganizationName("CanavarGraphicsEngine");

    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    DtedWindow window;
    window.setWindowTitle("Copernicus GLO-30 DEM Downloader  –  CanavarGraphicsEngine");
    window.show();

    return app.exec();
}
