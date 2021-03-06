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
        QVERIFY(source.addSeries(new DataSeries("Series 1")));
        QVERIFY(source.addSeries(new DataSeries("Series 2")));
        QVERIFY(source.addSeries(new DataSeries("Series 3 - cats")));
        QVERIFY(source.addSeries(new DataSeries("Series 4 - cats and dogs")));
        QVERIFY(source.addSeries(new DataSeries("Series 5")));

        QCOMPARE(source.getSeriesCount(), 5);

        QSharedPointer<DataSeries> ptr;

        // Extract source by index
        ptr = source.getSeriesByIndex(0);

        QVERIFY(!ptr.isNull());
        QCOMPARE((*ptr).getLabel(), "Series 1");

        // Try to add it back in again
        QVERIFY(!source.addSeries(ptr));
        QCOMPARE(source.getSeriesCount(), 5);

        // Extract by invalid index
        ptr = source.getSeriesByIndex(100);
        QVERIFY(ptr.isNull());

        // Extract by label
        ptr = source.getSeriesByLabel("Series 2");
        QVERIFY(!ptr.isNull());

        // Extract by invalid label
        ptr = source.getSeriesByLabel("Series 99");
        QVERIFY(ptr.isNull());

        // Extract series labels
        QStringList labels = source.getSeriesLabels();

        QCOMPARE(labels.size(), 5);

        QVERIFY(labels.contains("Series 1"));
        QVERIFY(labels.contains("Series 5"));

        // Extract series labels with filters
        labels = source.getSeriesLabels("cats");
        QCOMPARE(labels.size(), 2);

        labels = source.getSeriesLabels("cats dogs");
        QCOMPARE(labels.size(), 1);

        // Remove by index
        QVERIFY(source.removeSeriesByIndex(0));
        QCOMPARE(source.getSeriesCount(), 4);

        // Remove by invalid index
        QVERIFY(!source.removeSeriesByIndex(999));

        // Remove by label
        QVERIFY(source.removeSeriesByLabel("Series 5"));
        QCOMPARE(source.getSeriesCount(), 3);

        // Remove by invalid label
        QVERIFY(!source.removeSeriesByLabel("Series 5"));

        // Remove *all* series
        source.removeAllSeries();
        QCOMPARE(source.getSeriesCount(), 0);

    }

    void testGroups(void)
    {
        source.addSeries(new DataSeries("Animals", "Cat"));
        source.addSeries(new DataSeries("Animals", "Dog"));
        source.addSeries(new DataSeries("Animals", "Bird"));
        source.addSeries(new DataSeries("Animals", "Giraffe"));

        source.addSeries(new DataSeries("Plants", "Tree"));
        source.addSeries(new DataSeries("Plants", "Flower"));
        source.addSeries(new DataSeries("Plants", "Grass"));

        auto groups = source.getGroupLabels();

        QCOMPARE(groups.size(), 2);
    }

    // Tests for DataSourceManager class
    void testDataSourceManager(void)
    {
        auto *manager = DataSourceManager::getInstance();

        QCOMPARE(manager->getSourceCount(), 0);

        auto src_1 = QSharedPointer<DataSource>(new DataSource("Source 1"));

        // Test that a given source can only be added once
        QCOMPARE(manager->addSource(src_1), true);
        QCOMPARE(manager->addSource(src_1), false);
        QCOMPARE(manager->addSource(src_1), false);

        QCOMPARE(manager->getSourceCount(), 1);

        auto src_2 = QSharedPointer<DataSource>(new DataSource("Source 2"));
        auto src_3 = QSharedPointer<DataSource>(new DataSource("Source 3"));

        manager->addSource(src_2);
        manager->addSource(src_3);

        QCOMPARE(manager->getSourceCount(), 3);

        manager->removeSource(src_3);

        QCOMPARE(manager->getSourceCount(), 2);

        manager->addSource(src_3);

        QCOMPARE(manager->getSourceCount(), 3);

        // Extract source labels
        auto labels = manager->getSourceLabels();

        QCOMPARE(labels.size(), 3);
        QVERIFY(labels.contains("Source 1"));
        QVERIFY(labels.contains("Source 2"));
        QVERIFY(labels.contains("Source 3"));

        // Grab the pointer again
        manager = DataSourceManager::getInstance();

        QCOMPARE(manager->getSourceCount(), 3);

        // Tests for source access!

        auto src = manager->getSourceByIndex(100);
        QVERIFY(src.isNull());

        src = manager->getSourceByLabel("Does not exist");
        QVERIFY(src.isNull());

        src = manager->getSourceByIndex(0);
        QVERIFY(!src.isNull());
        QCOMPARE(src->getLabel(), "Source 1");

        src = manager->getSourceByLabel("Source 2");
        QVERIFY(!src.isNull());
        QCOMPARE(src->getLabel(), "Source 2");

        // Test removal functions
        QCOMPARE(manager->removeSourceByIndex(500), false);
        QCOMPARE(manager->getSourceCount(), 3);

        QCOMPARE(manager->removeSourceByLabel("NOOO"), false);
        QCOMPARE(manager->getSourceCount(), 3);

        QCOMPARE(manager->removeSourceByIndex(0), true);
        QCOMPARE(manager->removeSourceByLabel("Source 1"), false);
        QCOMPARE(manager->removeSourceByLabel("Source 2"), true);

        QCOMPARE(manager->getSourceCount(), 1);

        // Test clearing all sources
        manager->removeAllSources();
        QCOMPARE(manager->getSourceCount(), 0);
    }

protected:
    DataSource source;
};

#endif // TEST_SOURCE_HPP
