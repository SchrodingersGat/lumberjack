#include <algorithm>

#include "data_series.hpp"
#include "data_source.hpp"


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


DataSeries::DataSeries()
{
    clearData();
}


DataSeries::DataSeries(QString lbl, QString u) : DataSeries()
{
    label = lbl;
    units = u;
}


// Copy from another data series
DataSeries::DataSeries(const DataSeries &other) : DataSeries()
{
    label = other.getLabel();
    units = other.getUnits();

    data = other.getData();

    // TODO - What else needs copying?

    update();
}


// Copy from another data series, within the specified time range
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

void DataSeries::update(void)
{
    // TODO - What needs to happen here?

    emit dataUpdated();
}


DataSeries::~DataSeries()
{
    clearData();
}


size_t DataSeries::size() const
{
    return data.size();
}

QVector<DataPoint> DataSeries::getData(void) const
{
    return data;
}


QVector<DataPoint> DataSeries::getData(double t_min, double t_max) const
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

    return data.mid(idx_min, idx_max - idx_min);
}


DataPoint DataSeries::getDataPoint(uint64_t idx) const
{
    if (idx >= size())
    {
        throw std::out_of_range("data index out of range");
    }

    return data[idx];
}


double DataSeries::getTimestamp(uint64_t idx) const
{
    return getDataPoint(idx).timestamp;
}


float DataSeries::getValue(uint64_t idx) const
{
    return getDataPoint(idx).value;
}


void DataSeries::addData(DataPoint point, bool do_update)
{
    data_mutex.lock();

    if (size() == 0 || point.timestamp > getNewestTimestamp())
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

    auto span = idx_max - idx_min;

    data = data.mid(idx_min, span);

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


DataPoint DataSeries::getOldestDataPoint() const
{
    if (size() > 0)
    {
        return data.front();
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


DataPoint DataSeries::getNewestDataPoint() const
{
    if (size() > 0)
    {
        return data.back();
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
    // TODO
    return 0;
}


double DataSeries::getMaximumValue() const
{
    // TODO
    return 0;
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
    auto idx_min = getIndexForTimestamp(t_min, SEARCH_RIGHT_TO_LEFT);
    auto idx_max = getIndexForTimestamp(t_max, SEARCH_RIGHT_TO_LEFT);

    double accumulator = 0;
    unsigned int count = 0;

    unsigned int length = size();

    for (size_t idx = idx_min; idx <= idx_max; idx++)
    {
        if (idx < length)
        {
            accumulator += getValue(idx);
            count += 1;
        }
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
