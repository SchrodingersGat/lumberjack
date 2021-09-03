#ifndef DATA_SERIES_H
#define DATA_SERIES_H

#include <qobject.h>
#include <qvector.h>
#include <qmutex.h>
#include <QRectF>
#include <QColor>

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
    DataSeries(QString label);
    DataSeries(QString group, QString label);
    DataSeries(const DataSeries& other);
    DataSeries(const DataSeries& other, int64_t t_min, int64_t t_max, unsigned int expand=0);

//    DataSeries& operator=(const DataSeries& other);

    virtual ~DataSeries();

    enum SearchDirection
    {
        SEARCH_LEFT_TO_RIGHT,
        SEARCH_RIGHT_TO_LEFT,
    };

    enum InterpolationMode
    {
        INTERPOLATE,
        SAMPLE_HOLD,
    };

public slots:

    const QString& getGroup(void) const { return group; }
    void setGroup(QString g) { group = g; }

    const QString& getLabel(void) const { return label; }
    void setLabel(QString l) { label = l; }

    const QString& getUnits(void) const { return units; }
    void setUnits(QString u) { units = u; }

    QColor getColor(void) const { return color; }
    void setColor(QColor c);

    /* Data insertion functions */
    void addData(DataPoint point, bool update=true);
    void addData(double t_ms, float value, bool update=true);

    void clipTimeRange(double t_min, double t_max, bool update=true);

    /* Data removal functions */
    void clearData(bool update=true);

    /* Data access functions */
    size_t size() const;

    QRectF getBounds(void) const;

    QVector<DataPoint> getData() const;
    QVector<DataPoint> getData(double t_min, double t_max) const;

    const DataPoint& getDataPoint(uint64_t idx) const;
    double getTimestamp(uint64_t idx) const;
    double getValue(uint64_t idx) const;

    const DataPoint& getOldestDataPoint(void) const;
    double getOldestTimestamp(void) const;
    double getOldestValue(void) const;

    const DataPoint& getNewestDataPoint(void) const;
    double getNewestTimestamp(void) const;
    double getNewestValue(void) const;

    double getMinimumValue(void) const;
    double getMinimumValue(double t_min, double t_max) const;

    double getMaximumValue(void) const;
    double getMaximumValue(double t_min, double t_max) const;

    double getValueAtTime(double timestamp, InterpolationMode mode=INTERPOLATE) const;

    double getMeanValue(void) const;
    double getMeanValue(double t_min, double t_max) const;

    uint64_t getIndexForTimestamp(double t, SearchDirection direction=SEARCH_LEFT_TO_RIGHT) const;

    /* Status Functions */
    bool hasData() const { return size() > 0; }

    void update(void) { emit dataUpdated(); }
    void updateStyle(void) { emit styleUpdated(); }

signals:
    // Emitted when data are updated
    void dataUpdated();
    void styleUpdated();

protected:

    QVector<DataPoint> data;

    //! mutex for controlling data access
    mutable QMutex data_mutex;

    //! Group string for this DataSeries
    QString group;

    //! Label string for this DataSeries
    QString label;

    //! Units string for this DataSeries
    QString units;

    //! Color for this curve
    QColor color;
};


#endif // DATA_SERIES_H
