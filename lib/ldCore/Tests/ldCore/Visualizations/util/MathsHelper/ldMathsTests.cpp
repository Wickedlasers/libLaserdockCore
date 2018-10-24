#include "ldMathsTests.h"

#include <QtTest/QTest>

#include "ldCore/Visualizations/util/MathsHelper/ldMaths.h"

void ldMathsTests::testPeriodIntervalKeeperInt()
{
    testPeriodIntervalKeeperInt(INT_MIN, 0, 360);
    testPeriodIntervalKeeperInt(0, 0, 360);
    testPeriodIntervalKeeperInt(180, 0, 360);
    testPeriodIntervalKeeperInt(360, 0, 360);
    testPeriodIntervalKeeperInt(INT_MAX, 0, 360);

    testPeriodIntervalKeeperInt(INT_MIN, -180, 360);
    testPeriodIntervalKeeperInt(-180, -180, 360);
    testPeriodIntervalKeeperInt(0, -180, 360);
    testPeriodIntervalKeeperInt(360, -180, 360);
    testPeriodIntervalKeeperInt(INT_MAX, -180, 360);
}

void ldMathsTests::testPeriodIntervalKeeperInt(int x, int min, int max)
{
    int res = ldMaths::periodIntervalKeeperInt(x, min, max);
    int oldRes = periodIntervalKeeperIntOld(x, min, max);
    QCOMPARE(res, oldRes);
}

int ldMathsTests::periodIntervalKeeperIntOld(int x, int min, int max)
{
    int len=max-min;
    while (x>max) x=x-len;
    while (x<min) x=x+len;
    return x;
}

QTEST_MAIN(ldMathsTests)
