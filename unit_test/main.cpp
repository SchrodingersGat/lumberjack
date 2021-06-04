#include <qtest.h>

#include "test_series.hpp"
#include "test_source.hpp"

int main(int argc, char *argv[])
{
    int result = 0;

    DataSeriesTests test_series;
    result += QTest::qExec(&test_series, argc, argv);

    DataSourceTests test_source;
    result += QTest::qExec(&test_source, argc, argv);

    return result;
}
