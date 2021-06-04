#include <algorithm>

#include "series.hpp"


TimestampedData::TimestampedData()
{
    clearData();
}


TimestampedData::~TimestampedData()
{
    clearData();
}


size_t TimestampedData::size() const
{
    return t_data.size();
}


int64_t TimestampedData::getTimestamp(uint64_t idx) const
{
    return t_data.at(idx);
}


float TimestampedData::getValue(uint64_t idx) const
{
    return y_data.at(idx);
}


void TimestampedData::addData(int64_t t_ms, float value)
{
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
}


void TimestampedData::clearData()
{
    data_mutex.lock();

    t_data.clear();
    y_data.clear();

    data_mutex.unlock();
}


int64_t TimestampedData::getOldestTimestamp() const
{
    int64_t t = 0;

    if (size() > 0)
    {
        t = t_data.front();
    }

    return t;
}


int64_t TimestampedData::getNewestTimestamp() const
{
    int64_t t = 0;

    if (size() > 0)
    {
        t = t_data.back();
    }

    return t;
}


float TimestampedData::getOldestValue() const
{
    float v = 0;

    if (size() > 0)
    {
        v = y_data.front();
    }

    return v;
}


float TimestampedData::getNewestValue() const
{
    float v = 0;

    if (size() > 0)
    {
        v = y_data.back();
    }

    return v;
}


float TimestampedData::getMinimumValue() const
{
    // TODO
    return 0;
}


float TimestampedData::getMaximumValue() const
{
    // TODO
    return 0;
}


uint64_t TimestampedData::getIndexForTimestamp(int64_t t_ms, SearchDirection direction)
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
