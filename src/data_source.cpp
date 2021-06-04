#include "data_source.hpp"

DataSource::DataSource(QString lbl) :
    label(lbl)
{
    // TODO
}


DataSource::~DataSource()
{
    clear();
}


void DataSource::clear()
{
    // TODO
}


QStringList DataSource::getSeriesLabels(void) const
{
    QStringList labels;

    for (auto series : data_series)
    {
        if (series.isNull())
        {
            continue;
        }

        labels.append((*series).getLabel());
    }

    return labels;
}


QStringList DataSource::getSourceLabels() const
{
    QStringList labels;

    for (auto source : data_sources)
    {
        if (source.isNull())
        {
            continue;
        }

        labels.append((*source).getLabel());
    }

    return labels;
}


/*
 * Add the provided DataSeries to the list of series associated with this source.
 * If the series is null, or already present, it will not be added.
 *
 * Returns true if the series was added, else false
 */
bool DataSource::addSeries(QSharedPointer<DataSeries> series)
{
    if (series.isNull())
    {
        return false;
    }

    for (auto s: data_series)
    {
        if (s == series)
        {
            return false;
        }
    }

    data_series.push_back(series);

    return true;
}


bool DataSource::addSeries(DataSeries *series)
{
    return addSeries(QSharedPointer<DataSeries>(series));
}


/*
 * Return a pointer to the DataSeries at the specified index.
 *
 * If the index is out of bounds, return a null DataSeries pointer
 */
QSharedPointer<DataSeries> DataSource::getSeriesByIndex(unsigned int index)
{
    if (index < data_series.size())
    {
        return data_series.at(index);
    }

    return QSharedPointer<DataSeries>(nullptr);
}


/*
 * Return a pointer to the DataSeries with the specified label.
 *
 * If the label is not found, return a null DataSeries pointer
 */
QSharedPointer<DataSeries> DataSource::getSeriesByLabel(QString label)
{
    for (auto s : data_series)
    {
        if (!s.isNull() && s->getLabel() == label)
        {
            return s;
        }
    }

    return QSharedPointer<DataSeries>(nullptr);
}
