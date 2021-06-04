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
}


DataSeries::~DataSeries()
{
    clearData();
}


size_t DataSeries::size() const
{
    return t_data.size();
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


void DataSeries::addData(int64_t t_ms, float value, bool update)
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

    if (update)
    {
        emit dataUpdated();
    }

    data_mutex.unlock();
}


void DataSeries::clearData(bool update)
{
    data_mutex.lock();

    t_data.clear();
    y_data.clear();

    data_mutex.unlock();

    if (update)
    {
        emit dataUpdated();
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


uint64_t DataSeries::getIndexForTimestamp(int64_t t_ms, SearchDirection direction)
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
