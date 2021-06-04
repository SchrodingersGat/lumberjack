#include <algorithm>

#include "data_series.hpp"
#include "data_source.hpp"


DataSeries::DataSeries(QString lbl, QString u) :
    label(lbl),
    units(u)
{
    clearData();
}


// Copy from another data series
DataSeries::DataSeries(const DataSeries &other)
{
    label = other.getLabel();
    units = other.getUnits();

    t_data = other.getTimestampData();
    y_data = other.getValueData();

    // TODO - What else needs copying?

    update();
}


// Copy from another data series, within the specified time range
DataSeries::DataSeries(const DataSeries &other, int64_t t_min, int64_t t_max, unsigned int expand)
{
    label = other.getLabel();
    units = other.getUnits();

    // Ensure that the timestamps are the right way around!
    if (t_min > t_max)
    {
        int64_t swap = t_min;

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
    t_data.reserve(idx_max - idx_min);
    y_data.reserve(idx_max - idx_min);

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


DataSeries::~DataSeries()
{
    clearData();
}


size_t DataSeries::size() const
{
    return t_data.size();
}


void DataSeries::update(void)
{
    // TODO - What needs to happen here?

    emit dataUpdated();
}


int64_t DataSeries::getTimestamp(uint64_t idx) const
{
    if (idx >= size())
    {
        throw std::out_of_range("Index out of range for DataSeries::getTimestamp");
    }

    return t_data.at(idx);
}


QVector<int64_t> DataSeries::getTimestampData(void) const
{
    return t_data;
}


float DataSeries::getValue(uint64_t idx) const
{
    if (idx >= size())
    {
        throw std::out_of_range("Index out of range for DataSeries::getValue");
    }

    return y_data.at(idx);
}


QVector<float> DataSeries::getValueData(void) const
{
    return y_data;
}


void DataSeries::addData(int64_t t_ms, float value, bool do_update)
{
    data_mutex.lock();

    // Quicker if the timestamps are added in order
    if (t_ms >= getNewestTimestamp())
    {
        t_data.push_back(t_ms);
        y_data.push_back(value);
    }
    else
    {
        auto idx = getIndexForTimestamp(t_ms);

        t_data.insert(t_data.begin() + idx, t_ms);
        y_data.insert(y_data.begin() + idx, value);
    }

    if (do_update)
    {
        update();
    }

    data_mutex.unlock();
}


void DataSeries::clearData(bool do_update)
{
    data_mutex.lock();

    t_data.clear();
    y_data.clear();

    data_mutex.unlock();

    if (do_update)
    {
        update();
    }
}


int64_t DataSeries::getOldestTimestamp() const
{
    int64_t t = 0;

    if (size() > 0)
    {
        t = t_data.front();
    }

    return t;
}


int64_t DataSeries::getNewestTimestamp() const
{
    int64_t t = 0;

    if (size() > 0)
    {
        t = t_data.back();
    }

    return t;
}


float DataSeries::getOldestValue() const
{
    float v = 0;

    if (size() > 0)
    {
        v = y_data.front();
    }

    return v;
}


float DataSeries::getNewestValue() const
{
    float v = 0;

    if (size() > 0)
    {
        v = y_data.back();
    }

    return v;
}


float DataSeries::getMinimumValue() const
{
    // TODO
    return 0;
}


float DataSeries::getMaximumValue() const
{
    // TODO
    return 0;
}


uint64_t DataSeries::getIndexForTimestamp(int64_t t_ms, SearchDirection direction) const
{
    if (t_ms < getOldestTimestamp())
    {
        return 0;
    }

    else if (t_ms > getNewestTimestamp())
    {
        return size();
    }

    if (direction == SEARCH_LEFT_TO_RIGHT)
    {
        auto upper= std::upper_bound(t_data.begin(), t_data.end(), t_ms);
        return std::distance(t_data.begin(), upper);
    }
    else
    {
        auto lower = std::lower_bound(t_data.begin(), t_data.end(), t_ms);
        return std::distance(t_data.begin(), lower);
    }
}
