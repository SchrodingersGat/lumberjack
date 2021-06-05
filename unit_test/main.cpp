#include <qtest.h>

#include "test_series.hpp"
#include "test_source.hpp"
#include "test_curve.hpp"

int main(int argc, char *argv[])
{
    int result = 0;

    qDebug() << "Running unit tests for DataSeries class";

    DataSeriesTests test_series;
    result += QTest::qExec(&test_series, argc, argv);

    qDebug() << "Running unit tests for DataSource class";

    DataSourceTests test_source;
    result += QTest::qExec(&test_source, argc, argv);

    qDebug() << "Running unit tests for PlotCurve class";

    PlotCurveTests test_curve;
    result += QTest::qExec(&test_curve, argc, argv);

    qDebug() << "All tests complete" << result;

    return result;
}
