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


QStringList DataSource::getSeriesLabels(QString filter_string) const
{
    QStringList labels;

    QList<QRegExp> patterns;

    for (QString pattern : filter_string.trimmed().split(" "))
    {
        if (!pattern.startsWith("*"))
        {
            pattern.prepend("*");
        }

        if (!pattern.endsWith("*"))
        {
            pattern.append("*");
        }

        patterns.append(QRegExp(pattern, Qt::CaseInsensitive, QRegExp::Wildcard));
    }

    for (auto series : data_series)
    {
        if (series.isNull())
        {
            continue;
        }

        QString label = (*series).getLabel();

        // Test if the label matches *ALL* the filters
        bool matches_all = true;

        for (auto pattern : patterns)
        {
            if (!pattern.exactMatch(label))
            {
                matches_all = false;
                break;
            }
        }

        if (matches_all)
        {
            labels.append(label);
        }
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


bool DataSource::removeSeriesByIndex(unsigned int index)
{
    if (index < data_series.size())
    {
        data_series.removeAt(index);
        return true;
    }

    return false;
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


bool DataSource::removeSeriesByLabel(QString label)
{
    for (int idx = 0; idx < data_series.size(); idx++)
    {
        auto series = data_series.at(idx);

        if (!series.isNull() && (*series).getLabel() == label)
        {
            data_series.removeAt(idx);
            return true;
        }
    }

    return false;
}


void DataSource::removeAllSeries(void)
{
    while (data_series.size() > 0)
    {
        removeSeriesByIndex(0);
    }
}


/*
 * Construct a list of all "group" labels within this dataset
 */
QStringList DataSource::getGroupLabels() const
{
    QStringList labels;

    for (auto series : data_series)
    {
        if (!series.isNull() && !labels.contains(series->getGroup()))
        {
            labels.append(series->getGroup());
        }
    }

    return labels;
}


DataSourceManager *DataSourceManager::instance = 0;


DataSourceManager::DataSourceManager()
{
    // TODO
}


QStringList DataSourceManager::getSourceLabels() const
{
    QStringList labels;

    for (auto src : sources)
    {
        if (src.isNull()) continue;

        labels.push_back(src->getLabel());
    }

    return labels;
}


QSharedPointer<DataSource> DataSourceManager::getSourceByIndex(int idx)
{
    if (idx < sources.size()) return sources.at(idx);

    return QSharedPointer<DataSource>(nullptr);
}


QSharedPointer<DataSource> DataSourceManager::getSourceByLabel(QString label)
{
    for (auto src : sources)
    {
        if (src.isNull()) continue;

        if (src->getLabel() == label)
        {
            return src;
        }
    }

    return QSharedPointer<DataSource>(nullptr);
}


bool DataSourceManager::addSource(QSharedPointer<DataSource> source)
{
    if (source.isNull()) return false;

    for (auto src : sources)
    {
        if (src == source)
        {
            qInfo() << "Ignoring duplicate source:" << source->getLabel();

            return false;
        }
    }

    sources.push_back(source);

    return true;
}


bool DataSourceManager::removeSource(QSharedPointer<DataSource> source)
{
    for (auto idx = 0; idx < sources.size(); idx++)
    {
        auto src = sources.at(idx);

        if (src == source)
        {
            sources.removeAt(idx);
            return true;
        }
    }

    return false;
}
