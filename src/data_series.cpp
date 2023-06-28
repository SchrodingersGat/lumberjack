#include <math.h>
#include <algorithm>

#include "data_series.hpp"
#include "data_source.hpp"


const float DataSeries::LINE_WIDTH_MIN = 1.0f;
const float DataSeries::LINE_WIDTH_MAX = 5.0f;

const unsigned int DataSeries::SYMBOL_SIZE_MIN = 3;
const unsigned int DataSeries::SYMBOL_SIZE_MAX = 10;


// Comparison operators for DataPoint and timestamps
bool operator== (const DataPoint point, const double timestamp)
{
    return point.timestamp == timestamp;
}

bool operator< (const DataPoint point, const double timestamp)
{
    return point.timestamp < timestamp;
}

bool operator< (const double timestamp, const DataPoint point)
{
    return timestamp < point.timestamp;
}

bool operator> (const DataPoint point, const double timestamp)
{
    return point.timestamp > timestamp;
}

bool operator> (const double timestamp, const DataPoint point)
{
    return timestamp > point.timestamp;
}


DataSeries::~DataSeries()
{
    clearData(false);
}


/*
 * Construct a new empty data series
 */
DataSeries::DataSeries()
{
    clearData();
}


/*
 * Construct a new empty data series with the specified label
 */
DataSeries::DataSeries(QString lbl) : DataSeries()
{
    label = lbl;
}


DataSeries::DataSeries(QString grp, QString lbl) : DataSeries()
{
    group = grp;
    label = lbl;
}


/*
 * Construct a new DataSeries, and copy data from another DataSeries
 */
DataSeries::DataSeries(const DataSeries &other) : DataSeries()
{
    label = other.getLabel();
    units = other.getUnits();

    data = other.getData();

    // TODO - What else needs copying?

    update();
}


/*
 * Construct a DataSeries by copying from another DataSeries,
 * within the specified time range
 */
DataSeries::DataSeries(const DataSeries &other, int64_t t_min, int64_t t_max, unsigned int expand) : DataSeries()
{
    label = other.getLabel();
    units = other.getUnits();

    // Ensure that the timestamps are the right way around!
    if (t_min > t_max)
    {
        double swap = t_min;

        t_min = t_max;
        t_max = swap;
    }

    // Construct a subsection
    auto idx_min = other.getIndexForTimestamp(t_min, SEARCH_RIGHT_TO_LEFT);
    auto idx_max = other.getIndexForTimestamp(t_max, SEARCH_RIGHT_TO_LEFT);

    // Attempt to expand the indices by a single count on either side
    while (expand > 0)
    {
        if (idx_min > 0)
        {
            idx_min--;
        }

        if (idx_max < (other.size() - 1))
        {
            idx_max++;
        }

        expand--;
    }

    // Allocate memory
    data.reserve(idx_max - idx_min);

    auto length = other.size();

    for (uint64_t idx = idx_min; idx <= idx_max; idx++)
    {
        if (idx < length)
        {
            addData(other.getTimestamp(idx), other.getValue(idx));
        }
    }

    update();
}


/*
 * Set the color of this DataSeries
 */
void DataSeries::setColor(QColor c)
{
    color = c;

    update();
}


/*
 * Return the size (number of samples) of this DataSeries
 */
size_t DataSeries::size() const
{
    return data.size();
}


/*
 * Calculate the bounds of this DataSeries.
 * Returns a QRectF instance
 */
QRectF DataSeries::getBounds() const
{
    if (size() == 0) return QRectF();

    auto oldest = getOldestDataPoint();
    auto newest = getNewestDataPoint();

    return QRectF(
                oldest.timestamp,
                qMax(oldest.value, newest.value),
                newest.timestamp - newest.timestamp,
                qMin(oldest.value, newest.value)
                );
}


std::vector<DataPoint> DataSeries::getData(void) const
{
    return data;
}


std::vector<DataPoint> DataSeries::getData(double t_min, double t_max) const
{
    // Ensure that the timestamps are the right way around!
    if (t_min > t_max)
    {
        double swap = t_min;

        t_min = t_max;
        t_max = swap;
    }

    // Subsection
    auto idx_min = getIndexForTimestamp(t_min, SEARCH_RIGHT_TO_LEFT);
    auto idx_max = getIndexForTimestamp(t_max, SEARCH_RIGHT_TO_LEFT);

    // Construct a subset of the data
    std::vector<DataPoint> subset;

    subset.resize(idx_max - idx_min + 2);

    for (uint64_t idx = idx_min; idx <= idx_max; idx++)
    {
        subset.push_back(data[idx]);
    }

    return subset;
}


const DataPoint DataSeries::getDataPoint(uint64_t idx) const
{
    if (idx >= size())
    {
        throw std::out_of_range("data index out of range");
    }

    DataPoint dp = data[idx];

    dp.value *= scalerValue;
    dp.value += offsetValue;

    return dp;
}


double DataSeries::getTimestamp(uint64_t idx) const
{
    return getDataPoint(idx).timestamp;
}


double DataSeries::getValue(uint64_t idx) const
{
    return getDataPoint(idx).value;
}


/*
 * Insert a new sample into this DataSeries.
 *
 * If the timestamp is "newer" than the most recent point in the series,
 * the new sample is simply appended to the dataset.
 * (This is a much more efficient operation).
 *
 * Otherwise, the sample is inserted into the series, which is more expensive.
 */
void DataSeries::addData(DataPoint point, bool do_update)
{
    // Ignore NaN values
    if (isnan(point.value)) return;

    // Ignore inf values
    if (isinf(point.value)) return;

    data_mutex.lock();

    // If the new datapoint is of equal or greater timestamp value, simply append!
    if (size() == 0 || point.timestamp >= getNewestTimestamp())
    {
        data.push_back(point);
    }
    else
    {
        auto idx = getIndexForTimestamp(point.timestamp);

        data.insert(data.begin() + idx, point);
    }

    if (do_update)
    {
        update();
    }

    data_mutex.unlock();
}


/*
 * Add a single data point to the series
 */
void DataSeries::addData(double t, float value, bool do_update)
{
    addData(DataPoint(t, value), do_update);
}


void DataSeries::clipTimeRange(double t_min, double t_max, bool do_update)
{
    // Ensure that the timestamps are the right way around!
    if (t_min > t_max)
    {
        double swap = t_min;

        t_min = t_max;
        t_max = swap;
    }

    auto idx_min = getIndexForTimestamp(t_min, SEARCH_RIGHT_TO_LEFT);
    auto idx_max = getIndexForTimestamp(t_max, SEARCH_LEFT_TO_RIGHT);

    // Construct a subset of the data
    std::vector<DataPoint> subset;

    subset.resize(idx_max - idx_min + 2);

    for (uint64_t idx = idx_min; idx <= idx_max; idx++)
    {
        subset.push_back(data[idx]);
    }

    // Override original data
    data = subset;

    if (do_update)
    {
        update();
    }
}


void DataSeries::clearData(bool do_update)
{
    data_mutex.lock();

    data.clear();

    data_mutex.unlock();

    if (do_update)
    {
        update();
    }
}


const DataPoint DataSeries::getOldestDataPoint() const
{
    if (size() > 0)
    {
        return getDataPoint(0);
    }
    else
    {
        throw std::out_of_range("getOldestDataPoint called on empty TimeSeries");
    }
}


double DataSeries::getOldestTimestamp() const
{
    return getOldestDataPoint().timestamp;
}


double DataSeries::getOldestValue() const
{
    return getOldestDataPoint().value;
}


const DataPoint DataSeries::getNewestDataPoint() const
{
    if (size() > 0)
    {
        return getDataPoint(size() - 1);
    }
    else
    {
        throw std::out_of_range("getNewestDataPoint called on empty TimeSeries");
    }
}


double DataSeries::getNewestTimestamp() const
{
    return getNewestDataPoint().timestamp;
}


double DataSeries::getNewestValue() const
{
    return getNewestDataPoint().value;
}


double DataSeries::getMinimumValue() const
{
    return getMinimumValue(getOldestTimestamp(), getNewestTimestamp());
}


double DataSeries::getMinimumValue(double t_min, double t_max) const
{
    if (size() == 0) return 0;

    auto idx_min = getIndexForTimestamp(t_min, SEARCH_RIGHT_TO_LEFT);
    auto idx_max = getIndexForTimestamp(t_max, SEARCH_RIGHT_TO_LEFT);

    double value = __DBL_MAX__;

    unsigned int length = size();

    for (auto idx = idx_min + 1; idx <= idx_max && idx < size(); idx++)
    {
        if (idx >= length) continue;

        double v = getValue(idx);

        if (v < value)
        {
            value = v;
        }
    }

    return value;
}


double DataSeries::getMaximumValue() const
{
    return getMaximumValue(getOldestTimestamp(), getNewestTimestamp());
}


double DataSeries::getMaximumValue(double t_min, double t_max) const
{
    if (size() == 0) return 0;

    auto idx_min = getIndexForTimestamp(t_min, SEARCH_RIGHT_TO_LEFT);
    auto idx_max = getIndexForTimestamp(t_max, SEARCH_RIGHT_TO_LEFT);

    double value = __DBL_MIN__;

    unsigned int length = size();

    for (auto idx = idx_min + 1; idx <= idx_max && idx < size(); idx++)
    {
        if (idx >= length) continue;

        double v = getValue(idx);

        if (v > value)
        {
            value = v;
        }
    }

    return value;
}


double DataSeries::getValueAtTime(double timestamp, InterpolationMode mode) const
{
    if (size() == 0)
    {
        return 0;
    }

    auto idx = getIndexForTimestamp(timestamp);

    // Time is "before" the oldest timestamp
    if (idx <= 0)
    {
        return getOldestValue();
    }

    // Time is "after" the newest timestamp
    if (idx >= size())
    {
        return getNewestValue();
    }

    // Interpolate
    auto point_a = getDataPoint(idx - 1);
    auto point_b = getDataPoint(idx);

    switch (mode)
    {
    case INTERPOLATE:
        {
            double dt = point_b.timestamp - point_a.timestamp;
            double dv = point_b.value - point_a.value;

            return point_a.value + dv * (timestamp - point_a.timestamp) / dt;
        }
    case SAMPLE_HOLD:
    default:
        {
            // Simply return the previous "most recent" value
            return point_a.value;
        }
    }
}


double DataSeries::getMeanValue(void) const
{
    return getMeanValue(getOldestTimestamp(), getNewestTimestamp());
}


double DataSeries::getMeanValue(double t_min, double t_max) const
{
    if (size() == 0) return 0;

    auto idx_min = getIndexForTimestamp(t_min, SEARCH_RIGHT_TO_LEFT);
    auto idx_max = getIndexForTimestamp(t_max, SEARCH_RIGHT_TO_LEFT);

    double accumulator = 0;
    unsigned int count = 0;

    unsigned int length = size();

    for (size_t idx = idx_min; idx <= idx_max && idx < size(); idx++)
    {
        if (idx < length)
        {
            accumulator += getValue(idx);
            count += 1;
        }
    }

    if (count == 0)
    {
        // Prevent divide-by-zero errors
        return 0;
    }

    return accumulator / count;
}


uint64_t DataSeries::getIndexForTimestamp(double t, SearchDirection direction) const
{

    if (t < getOldestTimestamp())
    {
        return 0;
    }

    else if (t > getNewestTimestamp())
    {
        return size();
    }

    if (direction == SEARCH_LEFT_TO_RIGHT)
    {
        auto upper= std::upper_bound(data.begin(), data.end(), t);
        return std::distance(data.begin(), upper);
    }
    else
    {
        auto lower = std::lower_bound(data.begin(), data.end(), t);
        return std::distance(data.begin(), lower);
    }
}
