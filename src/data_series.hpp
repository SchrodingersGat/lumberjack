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
    DataSeries(QString label);
    virtual ~DataSeries();

    enum SearchDirection
    {
        SEARCH_LEFT_TO_RIGHT,
        SEARCH_RIGHT_TO_LEFT,
    };

public slots:

    QString getLabel(void) const { return label; }

    /* Data insertion functions */
    void addData(int64_t t_ms, float value, bool update=false);

    /* Data removal functions */
    void clearData(bool update=true);

    /* Data access functions */
    size_t size() const;

    int64_t getTimestamp(uint64_t idx) const;
    float getValue(uint64_t idx) const;

    int64_t getOldestTimestamp(void) const;
    int64_t getNewestTimestamp(void) const;

    float getOldestValue(void) const;
    float getNewestValue(void) const;

    float getMinimumValue(void) const;
    float getMaximumValue(void) const;

    uint64_t getIndexForTimestamp(int64_t t_ms, SearchDirection direction=SEARCH_LEFT_TO_RIGHT);

    /* Status Functions */
    bool hasData() const { return size() > 0; }

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
};


#endif // DATA_SERIES_H
