#include "Editor.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);

    Canavar::Editor::Editor editor;
    editor.Run();

    return app.exec();
}
