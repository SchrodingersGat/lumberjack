#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

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
    QStringList getSeriesLabels(void) const;

    QSharedPointer<DataSeries> getSeriesByIndex(unsigned int index);
    QSharedPointer<DataSeries> getSeriesByLabel(QString label);

    bool addSeries(QSharedPointer<DataSeries> series);
    bool addSeries(DataSeries* series);

    /* DataSource access functions */
    int getSourceCount(void) const { return data_sources.size(); }
    QStringList getSourceLabels(void) const;

protected:

    QString label;

    QVector<QSharedPointer<DataSeries>> data_series;
    QVector<QSharedPointer<DataSource>> data_sources;
};


#endif // DATA_SOURCE_H
