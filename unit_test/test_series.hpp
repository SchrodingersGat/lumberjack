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
            series.addData(rand() % 100, rand() % 1000, false);
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

    // Test that we can correctly slice out a subsection of data
    void testSubsection(void)
    {
        series.clearData();

        // Construct a series of timestamps {0:100}
        for (int idx = 0; idx <= 100; idx++)
        {
            series.addData(idx, rand() % 1000 - 500);
        }

        // We will slice out everything between {20:40}

        DataSeries slice_1 = DataSeries(series, 20, 40);

        QCOMPARE(slice_1.getOldestTimestamp(), 20);
        QCOMPARE(slice_1.getNewestTimestamp(), 40);

        // Slice in reverse, should get same result
        DataSeries slice_2 = DataSeries(series, 40, 20);

        QCOMPARE(slice_2.getOldestTimestamp(), 20);
        QCOMPARE(slice_2.getNewestTimestamp(), 40);

        // Slice across the lower-end
        DataSeries slice_3 = DataSeries(series, -100, 10);

        QCOMPARE(slice_3.size(), 11);
        QCOMPARE(slice_3.getOldestTimestamp(), 0);
        QCOMPARE(slice_3.getNewestTimestamp(), 10);

        // Slice across the upper end
        DataSeries slice_4 = DataSeries(series, 45, 1000);

        QCOMPARE(slice_4.size(), 56);
        QCOMPARE(slice_4.getOldestTimestamp(), 45);
        QCOMPARE(slice_4.getNewestTimestamp(), 100);

        // Slice in the middle, expand
        DataSeries slice_5 = DataSeries(series, 10, 66, 20);

        QCOMPARE(slice_5.getOldestTimestamp(), 0);
        QCOMPARE(slice_5.getNewestTimestamp(), 86);
    }

    // Test timespan clipping
    void testClip(void)
    {
        series.clipTimeRange(20, 94);

        QCOMPARE(series.getOldestTimestamp(), 20);
        QCOMPARE(series.getNewestTimestamp(), 94);
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

        for (size_t idx = 0; idx < series.size(); idx++)
        {
            int64_t t = series.getTimestamp(idx);

            if (t < ts) return false;

            ts = t;
        }

        return true;
    }
};

#endif // TEST_SERIES_H
