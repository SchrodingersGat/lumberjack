#ifndef DATA_SERIES_H
#define DATA_SERIES_H

#include <qobject.h>
#include <qvector.h>
#include <qmutex.h>

#include <qwt_series_data.h>


class DataPoint
{
public:
    DataPoint() : timestamp(0), value(0) {}
    DataPoint(double t, double v) : timestamp(t), value(v) {}

    //! Timestamp (milliseconds)
    double timestamp = 0;

    //! Value
    double value = 0;
};


class DataSeries : public QObject
{
    Q_OBJECT

public:
    DataSeries();
    DataSeries(QString label, QString units=QString());
    DataSeries(const DataSeries& other);
    DataSeries(const DataSeries& other, int64_t t_min, int64_t t_max, unsigned int expand=0);

//    DataSeries& operator=(const DataSeries& other);

    virtual ~DataSeries();

    enum SearchDirection
    {
        SEARCH_LEFT_TO_RIGHT,
        SEARCH_RIGHT_TO_LEFT,
    };

public slots:

    QString getLabel(void) const { return label; }

    QString getUnits(void) const { return units; }

    /* Data insertion functions */
    void addData(DataPoint point, bool update=true);
    void addData(double t_ms, float value, bool update=true);

    void clipTimeRange(double t_min, double t_max, bool update=true);

    /* Data removal functions */
    void clearData(bool update=true);

    /* Data access functions */
    size_t size() const;

    QVector<DataPoint> getData() const;
    QVector<DataPoint> getData(double t_min, double t_max) const;

    DataPoint getDataPoint(uint64_t idx) const;
    double getTimestamp(uint64_t idx) const;
    float getValue(uint64_t idx) const;

    DataPoint getOldestDataPoint(void) const;
    double getOldestTimestamp(void) const;
    double getOldestValue(void) const;

    DataPoint getNewestDataPoint(void) const;
    double getNewestTimestamp(void) const;
    double getNewestValue(void) const;

    double getMinimumValue(void) const;
    double getMaximumValue(void) const;

    double getMeanValue(void) const;
    double getMeanValue(double t_min, double t_max) const;

    uint64_t getIndexForTimestamp(double t, SearchDirection direction=SEARCH_LEFT_TO_RIGHT) const;

    /* Status Functions */
    bool hasData() const { return size() > 0; }

    void update(void);

signals:
    // Emitted when data are updated
    void dataUpdated();

protected:

    QVector<DataPoint> data;

    //! mutex for controlling data access
    mutable QMutex data_mutex;

    QString label;

    QString units;
};


#endif // DATA_SERIES_H
