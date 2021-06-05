#include <qtest.h>

#include "test_series.hpp"
#include "test_source.hpp"
#include "test_curve.hpp"

int main(int argc, char *argv[])
{
    int result = 0;

    DataSeriesTests test_series;
    result += QTest::qExec(&test_series, argc, argv);

    DataSourceTests test_source;
    result += QTest::qExec(&test_source, argc, argv);

    PlotCurveTests test_curve;
    result += QTest::qExec(&test_curve, argc, argv);

    return result;
}
