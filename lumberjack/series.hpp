#ifndef SERIES_HPP
#define SERIES_HPP

#include <stdint.h>
#include <vector>

#include <qpoint.h>
#include <qmutex.h>

#include <qwt_series_data.h>


class TimestampedData : public QwtSeriesData<QPointF>
{
public:
    TimestampedData();
    virtual ~TimestampedData();

    /* Functions required for QwtSeriesData
     * http://qwt.sourceforge.net/class_qwt_series_data.html
     *
     * virtual size_t size() const
     * virtual <T> sample(size_t i) const
     * virtual QRectF boundingRect() const
     */

    virtual size_t size() const override;
    virtual QPointF sample(size_t idx) const override;
    virtual QRectF boundingRect() const override;

    /* Data insertion functions */
    bool addData(int64_t t_ms, float value);

    /* Data removal functions */
    void clearData(void);

    /* Data access functions */
    int64_t getOldestTimestamp(void) const;
    int64_t getNewestTimestamp(void) const;

    float getOldestValue(void) const;
    float getNewestValue(void) const;

    float getMinimumValue(void) const;
    float getMaximumValue(void) const;

    /* Status Functions */
    bool hasData() const { return size() > 0; }

protected:

    //! time samples
    std::vector<float> t_data;
    //! value samples
    std::vector<float> y_data;

    //! mutex for controlling data access
    mutable QMutex data_mutex;

};


#endif // SERIES_HPP
