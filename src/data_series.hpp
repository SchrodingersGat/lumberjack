#ifndef DATA_SERIES_H
#define DATA_SERIES_H

#include <qobject.h>
#include <qvector.h>
#include <qmutex.h>

#include <qwt_series_data.h>

class DataSeries : public QObject
{
    Q_OBJECT

public:
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
    void addData(int64_t t_ms, float value, bool update=false);

    /* Data removal functions */
    void clearData(bool update=true);

    /* Data access functions */
    size_t size() const;

    int64_t getTimestamp(uint64_t idx) const;
    QVector<int64_t> getTimestampData(void) const;

    float getValue(uint64_t idx) const;
    QVector<float> getValueData(void) const;

    int64_t getOldestTimestamp(void) const;
    int64_t getNewestTimestamp(void) const;

    float getOldestValue(void) const;
    float getNewestValue(void) const;

    float getMinimumValue(void) const;
    float getMaximumValue(void) const;

    uint64_t getIndexForTimestamp(int64_t t_ms, SearchDirection direction=SEARCH_LEFT_TO_RIGHT) const;

    /* Status Functions */
    bool hasData() const { return size() > 0; }

    void update(void);

signals:
    // Emitted when data are updated
    void dataUpdated();

protected:

    //! time samples
    QVector<int64_t> t_data;
    //! value samples
    QVector<float> y_data;

    //! mutex for controlling data access
    mutable QMutex data_mutex;

    QString label;

    QString units;
};


#endif // DATA_SERIES_H
