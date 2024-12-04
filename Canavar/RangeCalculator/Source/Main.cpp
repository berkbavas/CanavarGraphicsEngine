#include "RangeCalculator.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    RangeCalculator::RangeCalculator calculator;

    calculator.Run();

    return app.exec();
}
