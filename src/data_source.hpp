#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include <QColor>
#include <qobject.h>
#include <qvector.h>

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

    void clear(void);

    QString getLabel(void) const { return label; }

    // Descriptive text for this data source (override for custom sources)
    virtual QString getDescription(void) const { return "Lumberjack data source"; }

    /* DataSeries access functions */
    int getSeriesCount(void) const { return data_series.size(); }
    QStringList getSeriesLabels(QString filter_string=QString()) const;

    QStringList getGroupLabels(void) const;

    bool addSeries(QSharedPointer<DataSeries> series, bool auto_color = true);
    bool addSeries(DataSeries* series, bool auto_color=true);

    QSharedPointer<DataSeries> getSeriesByIndex(unsigned int index);
    QSharedPointer<DataSeries> getSeriesByLabel(QString label);

    bool removeSeriesByIndex(unsigned int index);
    bool removeSeriesByLabel(QString label);

    void removeAllSeries(void);

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

    QVector<QSharedPointer<DataSeries>> data_series;
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

    static DataSourceManager *getInstance()
    {
        if (!instance)
        {
            instance = new DataSourceManager;
        }

        return instance;
    }

public slots:

    QSharedPointer<DataSeries> findSeries(QString source_label, QString series_label);

    int getSourceCount(void) const { return sources.size(); }
    QStringList getSourceLabels(void) const;

    QSharedPointer<DataSource> getSourceByIndex(int idx);
    QSharedPointer<DataSource> getSourceByLabel(QString label);

    bool addSource(QSharedPointer<DataSource> source);
    bool addSource(DataSource* source) { return addSource(QSharedPointer<DataSource>(source)); }
    bool addSource(QString label);

    bool removeSource(QSharedPointer<DataSource> source);
    bool removeSource(DataSource* source) { return removeSource(QSharedPointer<DataSource>(source)); }

    bool removeSourceByIndex(int idx);
    bool removeSourceByLabel(QString label);

    void removeAllSources(void);

    void update(void) { emit sourcesChanged(); }

signals:
    void sourcesChanged();

protected slots:
    void onDataChanged() { emit sourcesChanged(); }

protected:
    QVector<QSharedPointer<DataSource>> sources;
};


#endif // DATA_SOURCE_H
