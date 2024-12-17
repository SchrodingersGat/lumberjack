#ifndef DATA_SOURCE_MANAGER_HPP
#define DATA_SOURCE_MANAGER_HPP

#include <QThread>

#include "data_source.hpp"
#include "plugin_importer.hpp"
#include "plugin_exporter.hpp"


class DataIOWorker : public QObject
{
public:
    bool getResult(void) const { return m_result; }
    QStringList getErrors(void) const { return m_errors; }
    bool isComplete(void) const { return m_complete; }
protected:
    QStringList m_errors;
    bool m_complete = false;
    bool m_result = false;
};


/**
 * @brief The DataImportWorker class manages a data import session
 */
class DataImportWorker : public DataIOWorker
{
    Q_OBJECT

public:
    DataImportWorker(QSharedPointer<ImportPlugin> plugin);

public slots:
    void runImport(void);
    void cancelImport(void);

signals:
    void importCompleted(void);

protected:
    QSharedPointer<ImportPlugin> m_plugin;
};


class DataExportWorker : public DataIOWorker
{
    Q_OBJECT

public:
    DataExportWorker(QSharedPointer<ExportPlugin> plugin, QList<DataSeriesPointer> &series);

public slots:
    void runExport(void);
    void cancelExport(void);

signals:
    void exportCompleted(void);

protected:
    QSharedPointer<ExportPlugin> m_plugin;
    QList<DataSeriesPointer> m_series;
};


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
    bool addSource(QString source, QString label, QString description = QString());

    bool removeSource(DataSourcePointer source);
    bool removeSource(DataSource* source) { return removeSource(DataSourcePointer(source)); }

    bool removeSourceByIndex(unsigned int idx, bool update = true);
    bool removeSourceByLabel(QString label, bool update = true);

    void removeAllSources(bool update = true);

    // Data import functionality
    bool importData(QString filename = QString());

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
