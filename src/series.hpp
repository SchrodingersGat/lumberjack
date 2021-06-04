#ifndef SERIES_HPP
#define SERIES_HPP

#include <stdint.h>
#include <vector>
#include <mutex>


class TimestampedData
{
public:
    TimestampedData();
    virtual ~TimestampedData();

    /* Data insertion functions */
    bool addData(int64_t t_ms, float value);

    /* Data removal functions */
    void clearData(void);

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

    enum SearchDirection
    {
        SEARCH_LEFT_TO_RIGHT,
        SEARCH_RIGHT_TO_LEFT,
    };

    uint64_t getIndexForTimestamp(int64_t t_ms, SearchDirection direction, bool &result);

    /* Status Functions */
    bool hasData() const { return size() > 0; }

protected:

    //! time samples
    std::vector<float> t_data;
    //! value samples
    std::vector<float> y_data;

    //! mutex for controlling data access
    mutable std::mutex data_mutex;

};


#endif // SERIES_HPP
