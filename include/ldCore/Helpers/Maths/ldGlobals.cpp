#include "ldGlobals.h"


bool cmpf(float a, float b, float epsilon)
{
    return (fabsf(a - b) < epsilon);
}

