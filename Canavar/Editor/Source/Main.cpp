#include "Editor.h"

#include <QApplication>

using namespace Canavar::Editor;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Editor editor;
    editor.Run();

    return app.exec();
}
