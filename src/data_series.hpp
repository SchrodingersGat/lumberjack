#ifndef DATA_SERIES_H
#define DATA_SERIES_H

#include <stdint.h>
#include <vector>
#include <mutex>


// Forward-declaration of DataSource class
class DataSource;


class DataSeries
{
public:
    DataSeries(DataSource& src, std::string label);
    virtual ~DataSeries();

    std::string getLabel(void) const { return label; }

    /* Data insertion functions */
    void addData(int64_t t_ms, float value);

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

    uint64_t getIndexForTimestamp(int64_t t_ms, SearchDirection direction=SEARCH_LEFT_TO_RIGHT);

    /* Status Functions */
    bool hasData() const { return size() > 0; }

protected:

    //! time samples
    std::vector<int64_t> t_data;
    //! value samples
    std::vector<float> y_data;

    //! mutex for controlling data access
    mutable std::mutex data_mutex;

    DataSource &source;

    std::string label;
};


#endif // DATA_SERIES_H
