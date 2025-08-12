#include "Editor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Canavar::Editor::Editor *pEditor = new Canavar::Editor::Editor(nullptr);
    pEditor->Run();
    return  app.exec();
}
