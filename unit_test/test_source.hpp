#ifndef TEST_SOURCE_HPP
#define TEST_SOURCE_HPP

#include <stdlib.h>

#include <qobject.h>
#include <qtest.h>

#include "data_source.hpp"


class DataSourceTests : public QObject
{
    Q_OBJECT

public:
    DataSourceTests() : source("my data source") {}

private slots:
    void init(void)
    {
        // Test setup
    }

    void testDescriptors(void)
    {
        QCOMPARE(source.getLabel(), "my data source");
        QCOMPARE(source.getDescription(), "Lumberjack data source");
    }

    // Tests for DataSeries access functions
    void testSeriesFunctions(void)
    {
        // Initially there are no DataSeries associated with this DataSource
        QCOMPARE(source.getSeriesCount(), 0);

        // Add some series
        Q_ASSERT(source.addSeries(new DataSeries("Series 1")));
        Q_ASSERT(source.addSeries(new DataSeries("Series 2")));
        Q_ASSERT(source.addSeries(new DataSeries("Series 3 - cats")));
        Q_ASSERT(source.addSeries(new DataSeries("Series 4 - cats and dogs")));
        Q_ASSERT(source.addSeries(new DataSeries("Series 5")));

        QCOMPARE(source.getSeriesCount(), 5);

        QSharedPointer<DataSeries> ptr;

        // Extract source by index
        ptr = source.getSeriesByIndex(0);

        Q_ASSERT(!ptr.isNull());
        QCOMPARE((*ptr).getLabel(), "Series 1");

        // Try to add it back in again
        Q_ASSERT(!source.addSeries(ptr));
        QCOMPARE(source.getSeriesCount(), 5);

        // Extract by invalid index
        ptr = source.getSeriesByIndex(100);
        Q_ASSERT(ptr.isNull());

        // Extract by label
        ptr = source.getSeriesByLabel("Series 2");
        Q_ASSERT(!ptr.isNull());

        // Extract by invalid label
        ptr = source.getSeriesByLabel("Series 99");
        Q_ASSERT(ptr.isNull());

        // Extract series labels
        QStringList labels = source.getSeriesLabels();

        QCOMPARE(labels.size(), 5);

        Q_ASSERT(labels.contains("Series 1"));
        Q_ASSERT(labels.contains("Series 5"));

        // Extract series labels with filters
        labels = source.getSeriesLabels("cats");
        QCOMPARE(labels.size(), 2);

        labels = source.getSeriesLabels("cats dogs");
        QCOMPARE(labels.size(), 1);

        // Remove by index
        Q_ASSERT(source.removeSeriesByIndex(0));
        QCOMPARE(source.getSeriesCount(), 4);

        // Remove by invalid index
        Q_ASSERT(!source.removeSeriesByIndex(999));

        // Remove by label
        Q_ASSERT(source.removeSeriesByLabel("Series 5"));
        QCOMPARE(source.getSeriesCount(), 3);

        // Remove by invalid label
        Q_ASSERT(!source.removeSeriesByLabel("Series 5"));

        // Remove *all* series
        source.removeAllSeries();
        QCOMPARE(source.getSeriesCount(), 0);

    }

protected:
    DataSource source;
};

#endif // TEST_SOURCE_HPP
