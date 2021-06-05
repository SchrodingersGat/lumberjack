#ifndef TEST_CURVE_HPP
#define TEST_CURVE_HPP

#include <qobject.h>
#include <qtest.h>

#include "plot_curve.hpp"


class PlotCurveTests : public QObject
{
    Q_OBJECT

public:
    PlotCurveTests() : series("series")
    {
        PlotCurve *curve = new PlotCurve(&series);
    }

private slots:

    void testInitial(void)
    {
        QVERIFY(!curve.isNull());

        // TODO - Tests for curve downsampling
    }

protected:

    DataSeries series;
    QSharedPointer<PlotCurve> curve;
};

#endif // TEST_CURVE_HPP
