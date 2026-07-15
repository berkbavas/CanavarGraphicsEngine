
#include "Editor.h"

#include <QApplication>

int main(int Argc, char *pArgv[])
{
    QApplication Application(Argc, pArgv);
    Canavar::Editor::Editor Editor;
    Editor.Run();
    return Application.exec();
}
