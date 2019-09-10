#include "ldAnimatedAngle.h"

#include <ldCore/Helpers/Maths/ldMaths.h>

void ldAnimatedAngle::update(float minInterval, float maxInterval, bool keepdirection)
{
    value += sign*decay;
    if (!keepdirection) {
        if (value < minInterval || value > maxInterval) {
            sign = -sign;
            value += sign * decay;
            value += sign * decay;
        }
    }
    value = ldMaths::periodIntervalKeeper(value, minInterval, maxInterval);
}
