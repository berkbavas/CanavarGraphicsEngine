#include "GlobeWindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QSurfaceFormat>

int main(int Argc, char* Argv[])
{
    QApplication App(Argc, Argv);
    App.setApplicationName("Globe Viewer");
    App.setOrganizationName("CanavarGraphicsEngine");

    QCommandLineParser Parser;
    Parser.setApplicationDescription("WGS-84 Globe Terrain Viewer");
    Parser.addPositionalArgument("terrain", "Path to terrain root folder (contains index.tidx)");
    Parser.process(App);

    const QStringList Args = Parser.positionalArguments();
    const QString TerrainRoot = Args.isEmpty() ? "C:/Users/berkb/Desktop/Projects/CanavarGraphicsEngine/Terrain" : Args.first();

    QSurfaceFormat Fmt;
    Fmt.setDepthBufferSize(24);
    Fmt.setVersion(4, 5);
    Fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(Fmt);

    Canavar::Globe::GlobeWindow Window(TerrainRoot);
    Window.setWindowTitle(QString("Globe Viewer  [%1]  –  CanavarGraphicsEngine").arg(TerrainRoot));
    Window.show();

    return App.exec();
}
