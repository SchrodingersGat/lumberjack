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
    // TODO
    return false;
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


