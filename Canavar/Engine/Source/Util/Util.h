#pragma once

#include <QByteArray>
#include <QString>

namespace Canavar::Engine
{
    class Util
    {
      public:
        Util() = delete;

        static QByteArray GetBytes(const QString& path);
    };
}
