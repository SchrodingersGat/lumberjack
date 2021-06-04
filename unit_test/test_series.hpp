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

    // Test for data update signals
    void testDataSignals(void)
    {
        update_count = 0;

        connect(&series, SIGNAL(dataUpdated()), this, SLOT(onDataUpdated()));

        series.clearData();

        QCOMPARE(update_count, 1);

        for (int ii = 0; ii < 500; ii++)
        {
            series.addData(rand() % 100, rand() % 1000);
        }

        // Updates should not have fired
        QCOMPARE(update_count, 1);

        // Now add data, with updates on
        for (int jj = 0; jj < 10; jj++)
        {
            series.addData(rand() % 100, rand() % 1000, true);
        }

        QCOMPARE(update_count, 11);
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

        QVERIFY(isInOrder());

        // Add random samples (must be inserted in order!)

        for (int ii = 0; ii < 10000; ii++)
        {
            series.addData(rand() % 1000, rand() % 1000);
        }

        // Data must still be in correct timestamp order
        QVERIFY(isInOrder());
    }

    // Test that we can copy this series from another series
    void testCopy(void)
    {
        QCOMPARE(series.size(), 100);

        DataSeries copy = series;

        QCOMPARE(copy.size(), 100);

        // Clear the original series
        series.clearData();

        QCOMPARE(series.size(), 0);
        QCOMPARE(copy.size(), 100);
    }

public slots:
    void onDataUpdated()
    {
        update_count++;
    }

protected:

    int update_count = 0;

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
