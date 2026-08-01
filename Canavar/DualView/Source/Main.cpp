#include "DualView.h"

#include <QApplication>

int main(int Argc, char* pArgv[])
{
    QApplication Application(Argc, pArgv);
    Canavar::DualView::DualView DualView;
    DualView.Run();
    return Application.exec();
}
