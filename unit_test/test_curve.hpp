#ifndef TEST_CURVE_HPP
#define TEST_CURVE_HPP

#include <qobject.h>
#include <qtest.h>

#include "plot_curve.hpp"


class PlotCurveTests : public QObject
{
    Q_OBJECT

public:
    PlotCurveTests()
    {
        // Create a new DataSeries
        series = QSharedPointer<DataSeries>(new DataSeries("curve series"));

        // Create and store a new PlotCurve
        PlotCurve *plot_curve = new PlotCurve(series);
        curve = QSharedPointer<PlotCurve>(plot_curve);
    }

private slots:

    void testInitial(void)
    {
        QVERIFY(!curve.isNull());
    }

    void testDownsampling(void)
    {
        // Tests for down-sampling functionality

        // Generate a very large data set
        for (double t = 0; t <= 100; t += 0.0001)
        {
            series->addData(t, t * 100, false);
        }

        QCOMPARE(series->size(), 1000000);

        // Down-sample the data
        curve->resampleData(5, 15, 100);

        // Allow some time for the data to process
        waitMilliseconds(100);

        // Re-sample again!
        curve->resampleData(25, 95, 100);
    }

protected:

    void waitMilliseconds(int ms)
    {
        QTime timeout = QTime::currentTime().addMSecs(ms);

        while (QTime::currentTime() < timeout)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1);
        }
    }

    QSharedPointer<DataSeries> series;
    QSharedPointer<PlotCurve> curve;
};

#endif // TEST_CURVE_HPP
