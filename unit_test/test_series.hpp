#ifndef TEST_SERIES_H
#define TEST_SERIES_H

#include <stdlib.h>

#include <qobject.h>
#include <qtest.h>

#include "data_series.hpp"


class DataSeriesTests : public QObject
{
    Q_OBJECT

public:
    DataSeriesTests() : series("my series") {}

private slots:

    void init(void)
    {
        series.clearData();

        for (int idx = 0; idx < 100; idx++)
        {
            series.addData(idx, rand() % 100);
        }
    }

    // Tests for size functions
    void testSize(void)
    {
        // Initially 100 elements in the list
        QCOMPARE(series.size(), 100);

        // Clear data
        series.clearData();
        QCOMPARE(series.size(), 0);

        for (int idx = 0; idx < 10; idx++)
        {
            series.addData(idx, idx * 10);
            QCOMPARE(series.size(), idx + 1);
            QCOMPARE(series.getNewestTimestamp(), idx);
        }
    }

    // Tests for data accessor functions
    void testDataAccess(void)
    {
        QCOMPARE(series.getOldestTimestamp(), 0);
        QCOMPARE(series.getNewestTimestamp(), 99);

        for (size_t idx = 0; idx < series.size(); idx++)
        {
            QCOMPARE(series.getTimestamp(idx), idx);
        }

        QVERIFY_EXCEPTION_THROWN(series.getValue(1000), std::out_of_range);
    }

    // Tests for binary search indexing
    void testIndexing(void)
    {
        series.clearData();

        for (int idx = 0; idx < 100; idx++)
        {
            series.addData(idx * 10, 15 + idx * 100);
        }

        // Check insertion index for known values
        for (int ts = 0; ts < 990; ts++)
        {
            auto idx = series.getIndexForTimestamp(ts);

            QCOMPARE(idx, (int) ((ts + 10) / 10));
        }

        Q_ASSERT(isInOrder());

        // Add random samples (must be inserted in order!)

        for (int ii = 0; ii < 10000; ii++)
        {
            series.addData(rand() % 1000, rand() % 1000);
        }

        // Data must still be in correct timestamp order
        Q_ASSERT(isInOrder());
    }


protected:

    DataSeries series;

    // Determine if timestamp data are correctly ordered
    bool isInOrder(void) const
    {
        if (series.size() <= 1) return true;

        int64_t ts = series.getOldestTimestamp();

        for (int idx = 0; idx < series.size(); idx++)
        {
            int64_t t = series.getTimestamp(idx);

            if (t < ts) return false;

            ts = t;
        }

        return true;
    }
};

#endif // TEST_SERIES_H
