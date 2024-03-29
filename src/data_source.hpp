#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include <QColor>
#include <qobject.h>
#include <qvector.h>
#include <QFileInfo>

#include "data_series.hpp"


/**
 * @brief The DataSource class defines a "source" of data
 *
 * Each data source contains zero or more TimeSeries data objects,
 * and can also contain other sources, in a hierarchical fashion.
 *
 * This class can be sub-classed by plugins,
 * to enable loading data from custom sources;
 *
 * For example, loading data from non-standard file formats
 */
class DataSource : public QObject
{
    Q_OBJECT

public:

    DataSource(QString label);
    virtual ~DataSource();

    virtual QString getLabel(void) const { return label; }

    // Descriptive text for this data source (override for custom sources)
    virtual QString getDescription(void) const { return "Lumberjack data source"; }

    /* DataSeries access functions */
    int getSeriesCount(void) const { return data_series.size(); }
    QStringList getSeriesLabels(QString filter_string=QString()) const;

    QStringList getGroupLabels(void) const;

    bool addSeries(QSharedPointer<DataSeries> series, bool auto_color = true);
    bool addSeries(DataSeries* series, bool auto_color=true);
    bool addSeries(QString label, bool auto_color=true);

    QSharedPointer<DataSeries> getSeriesByIndex(unsigned int index);
    QSharedPointer<DataSeries> getSeriesByLabel(QString label);

    bool removeSeries(QSharedPointer<DataSeries> series, bool update = true);
    bool removeSeriesByLabel(QString label, bool update = true);

    void removeAllSeries(bool update = true);

signals:
    void dataChanged(void);

protected:

    //! Text label associated with this DataSource (e.g. filename)
    QString label;

    //! Circular list of colors to auto-assign to new series
    virtual QList<QColor> getColorWheel(void);

    QColor getNextColor(void);

    //! Cursor for selecting next color
    int color_wheel_cursor = 0;

    // Keep a map of label:series for efficient lookup
    QMap<QString, QSharedPointer<DataSeries>> data_series;
};


/**
 * @brief The FileDataSource class defines a DataSource which can be read from file
 */
class FileDataSource : public DataSource
{
    Q_OBJECT

public:
    FileDataSource(QString label);
    virtual ~FileDataSource();

    QStringList getFileHead(QString filename, int nLines = 10);

    bool loadData(QString filename, QStringList& errors);

    QString getFilename(void) const { return filename; }

    virtual QString getLabel(void) const override
    {
        return DataSource::getLabel() + ":" + QFileInfo(filename).fileName();
    }

    // These functions should be overridden for inheriting child classes
    virtual QString getFileDescription(void) const { return "Supported files"; }
    virtual QStringList getSupportedFileTypes(void) const { return QStringList(); }

    bool supportsFileType(QString suffix) const
    {
        return getSupportedFileTypes().contains(suffix.toLower());
    }

    // Function to set import options (called before loadDataFromFile)
    virtual bool setImportOptions(void) { return true; }

    virtual bool loadDataFromFile(QStringList &errors);

    QString getFilePattern(void) const;

protected:
    QString filename;

    bool validateFile(QString filename, QStringList& errors);

    qint64 getFileSize();
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

    QSharedPointer<DataSeries> findSeries(QString source_label, QString series_label);

    int getSourceCount(void) const { return sources.size(); }
    QStringList getSourceLabels(void) const;

    QSharedPointer<DataSource> getSourceByIndex(unsigned int idx);
    QSharedPointer<DataSource> getSourceByLabel(QString label);

    bool addSource(QSharedPointer<DataSource> source);
    bool addSource(DataSource* source) { return addSource(QSharedPointer<DataSource>(source)); }
    bool addSource(QString label);

    bool removeSource(QSharedPointer<DataSource> source);
    bool removeSource(DataSource* source) { return removeSource(QSharedPointer<DataSource>(source)); }

    bool removeSourceByIndex(unsigned int idx, bool update = true);
    bool removeSourceByLabel(QString label, bool update = true);

    void removeAllSources(bool update = true);

    void update(void) { emit sourcesChanged(); }

signals:
    void sourcesChanged();

protected slots:
    void onDataChanged() { emit sourcesChanged(); }

protected:
    QVector<QSharedPointer<DataSource>> sources;
};


#endif // DATA_SOURCE_H
