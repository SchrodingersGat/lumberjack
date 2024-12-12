#ifndef DATA_SOURCE_MANAGER_HPP
#define DATA_SOURCE_MANAGER_HPP

#include "data_source.hpp"

/*
 * Data source manager class:
 * - Manages all data sources
 * - Employs singleton design pattern so can be accessed globally
 */
class DataSourceManager : public QObject
{
    Q_OBJECT

    static DataSourceManager *instance;

public:
    DataSourceManager();
    ~DataSourceManager();

    static DataSourceManager *getInstance()
    {
        if (!instance)
        {
            instance = new DataSourceManager;
        }

        return instance;
    }

    static void cleanup()
    {
        if (instance)
        {
            delete instance;
            instance = nullptr;
        }
    }

public slots:

    DataSeriesPointer findSeries(QString source_label, QString series_label);

    int getSourceCount(void) const { return sources.size(); }
    QStringList getSourceLabels(void) const;

    DataSourcePointer getSourceByIndex(unsigned int idx);
    DataSourcePointer getSourceByLabel(QString label);

    bool addSource(DataSourcePointer source);
    bool addSource(DataSource* source) { return addSource(DataSourcePointer(source)); }
    bool addSource(QString label, QString description = QString());

    bool removeSource(DataSourcePointer source);
    bool removeSource(DataSource* source) { return removeSource(DataSourcePointer(source)); }

    bool removeSourceByIndex(unsigned int idx, bool update = true);
    bool removeSourceByLabel(QString label, bool update = true);

    void removeAllSources(bool update = true);

    // Data import functionality
    bool loadFromFile(QString filename = QString());

    // Data export functionality
    bool exportData(QList<DataSeriesPointer> &series, QString filename = QString());

    void update(void) { emit sourcesChanged(); }

signals:
    void sourcesChanged();

protected slots:
    void onDataChanged() { emit sourcesChanged(); }

protected:
    QVector<DataSourcePointer> sources;
};


#endif // DATA_SOURCE_MANAGER_HPP
