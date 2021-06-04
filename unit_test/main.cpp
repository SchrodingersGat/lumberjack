#include <qtest.h>

#include "test_series.hpp"
#include "test_source.hpp"

int main(int argc, char *argv[])
{
    DataSeriesTests test_series;
    QTest::qExec(&test_series, argc, argv);

    DataSourceTests test_source;
    QTest::qExec(&test_source, argc, argv);
}
