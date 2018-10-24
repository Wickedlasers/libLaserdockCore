#ifndef LDMATHSTESTS_H
#define LDMATHSTESTS_H

#include <QtCore/QObject>

class ldMathsTests : public QObject
{
    Q_OBJECT
private slots:
    // test int only version because float version has issue on max numbers
    void testPeriodIntervalKeeperInt();

private:
    void testPeriodIntervalKeeperInt(int x, int min, int max);

    // old function, not optimized, very slow on Android
    int periodIntervalKeeperIntOld(int x, int min, int max);
};

#endif // LDMATHSTESTS_H


