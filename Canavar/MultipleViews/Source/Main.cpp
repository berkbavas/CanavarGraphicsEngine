#include "MultipleViews.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Canavar::MultipleViews::MultipleViews *pMultipleViews = new Canavar::MultipleViews::MultipleViews(nullptr);
    pMultipleViews->Run();
    return app.exec();
}
