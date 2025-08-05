#include "Sun.h"

#include <cmath>

Canavar::Engine::Sun::Sun()
{
    SetNodeName("Sun");
    SetDirection(QVector3D(1, -1, 0).normalized());
}
