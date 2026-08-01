#include "ImageryWindow.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Satellite Imagery Downloader");
    app.setOrganizationName("CanavarGraphicsEngine");

    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    ImageryWindow window;
    window.setWindowTitle("Satellite Imagery Tile Downloader  –  CanavarGraphicsEngine");
    window.show();

    return app.exec();
}
