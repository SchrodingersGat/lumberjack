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


QRectF TimestampedData::boundingRect() const
{
    return QRectF(
                QPointF(getOldestTimestamp(), getMinimumValue()),
                QPointF(getNewestTimestamp(), getMaximumValue())
                );
}


QPointF TimestampedData::sample(size_t idx) const
{
    data_mutex.lock();

    if (idx < size())
    {
        QPointF pt((float) t_data[idx], y_data[idx]);
        data_mutex.unlock();
        return pt;
    }

    data_mutex.unlock();

    return QPointF(0, 0);
}


bool TimestampedData::addData(int64_t t_ms, float value)
{
    if (t_ms >= getNewestTimestamp())
    {
        t_data.push_back(t_ms);
        y_data.push_back(value);

        return true;
    }
    else
    {
        // TODO - Insert data into the array if out-of-order timestamps received
        return false;
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


/**
 * @brief TimestampedData::getIndexForTimestamp - Binary search for vector index matching given timestamp
 * @param t_ms
 * @param direction
 * @param result
 * @return
 */
int64_t TimestampedData::getIndexForTimestamp(int64_t t_ms, SearchDirection direction, bool &result)
{
    // Note: this function assumes that the data are sorted by timestamp

    if (size() == 0)
    {
        return 0;
    }

    // Quick checks for out-of-bounds
    if (t_ms < getOldestTimestamp())
    {
        return 0;
    }

    if (t_ms > getOldestTimestamp())
    {
        return size() - 1;
    }

    /*
    switch (direction)
    {
    case SEARCH_LEFT_TO_RIGHT:
        auto lower = std::lower_bound(t_data.begin(), t_data.end(), t_ms);
        break;
    case SEARCH_RIGHT_TO_LEFT:
        auto upper = std::upper_bound(t_data.begin(), t_data.end(), t_ms);
        break;
    default:
        // Unknwown
        return -1;
    }

    */

    // TODO
    return 0;
}

