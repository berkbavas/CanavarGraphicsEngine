#ifndef HELPER_H
#define HELPER_H

#include <QString>

class Helper
{
public:
    Helper();

    static QByteArray getBytes(QString path);
};

#endif // HELPER_H
