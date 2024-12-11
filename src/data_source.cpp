#include <QRegularExpression>
#include <QFile>
#include <QFileInfo>
#include <QDir>

#include "data_source.hpp"


DataSource::DataSource(QString label, QString description) :
    m_label(label),
    m_description(description)
{
}


DataSource::~DataSource()
{
    removeAllSeries(false);
}


/**
 * @brief DataSource::getColorWheel creates a list of "distinct" colors
 * @return a list of colors
 */
QList<QColor> DataSource::getColorWheel()
{
    QList<QColor> colors;

    // TODO: Make this ... better? ...

    colors.append(QColor(0, 0, 250));
    colors.append(QColor(0, 250, 0));
    colors.append(QColor(250, 0, 0));
    colors.append(QColor(0, 250, 250));
    colors.append(QColor(250, 250, 0));
    colors.append(QColor(250, 0, 250));
    colors.append(QColor(0, 0, 0));

    return colors;
}


/**
 * @brief DataSource::getNextColor get the next available color
 * @return
 */
QColor DataSource::getNextColor()
{
    auto colors = getColorWheel();

    if (colors.count() == 0)
    {
        return QColor(0, 0, 0);
    }

    int idx = color_wheel_cursor;

    if (idx >= colors.count()) idx = 0;

    QColor color = colors.at(idx);

    color_wheel_cursor = (idx + 1);

    return color;
}


/**
 * @brief DataSource::getSeriesLabels returns a list of labels for the DataSeries contained within this source
 * @param filter_string an (optional) wildcard filter to limit the returned labels
 * @return a list of zero or more labels
 */
QStringList DataSource::getSeriesLabels(QString filter_string) const
{
    QStringList labels;

    QList<QRegularExpression> patterns;

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

        // Convert from wildcard to regular expression
        QString re_pattern = QRegularExpression::wildcardToRegularExpression(pattern);
        patterns.append(QRegularExpression(re_pattern, QRegularExpression::CaseInsensitiveOption));
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
            if (!pattern.match(label).hasMatch())
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
bool DataSource::addSeries(DataSeriesPointer series, bool auto_color)
{
    if (series.isNull())
    {
        qWarning() << "Attempting to add null DataSeries";
        return false;
    }

    for (auto s : data_series.values())
    {
        if (s == series)
        {
            qWarning() << "Attempting to add duplicate DataSeries";
            return false;
        }
    }

    data_series[series->getLabel()] = series;

    if (auto_color)
    {
        series->setColor(getNextColor());
    }

    emit dataChanged();

    return true;
}


bool DataSource::addSeries(DataSeries *series, bool auto_color)
{
    return addSeries(DataSeriesPointer(series), auto_color);
}


bool DataSource::addSeries(QString label, bool auto_color)
{
    if (!getSeriesByLabel(label).isNull())
    {
        qWarning() << "Attempted to add duplicate series with label" << label;
        return false;
    }

    DataSeries* series = new DataSeries(label);

    return addSeries(series, auto_color);
}


/*
 * Return a pointer to the DataSeries at the specified index.
 *
 * If the index is out of bounds, return a null DataSeries pointer
 */
DataSeriesPointer DataSource::getSeriesByIndex(unsigned int index)
{
    QList<QString> keys = data_series.keys();

    if (index < (unsigned int) keys.length())
    {
        return data_series[keys[index]];
    }

    // No match found
    return DataSeriesPointer(nullptr);
}


bool DataSource::removeSeries(DataSeriesPointer series, bool update)
{
    for (QString label : data_series.keys())
    {
        if (data_series.value(label) == series)
        {
            data_series.remove(label);

            if (update)
            {
                emit dataChanged();
            }

            return true;
        }
    }

    // No matching series found
    return false;
}


/*
 * Return a pointer to the DataSeries with the specified label.
 *
 * If the label is not found, return a null DataSeries pointer
 */
DataSeriesPointer DataSource::getSeriesByLabel(QString label)
{
    if (data_series.contains(label))
    {
        return data_series[label];
    }

    // No match found - return a null series
    return DataSeriesPointer(nullptr);
}


bool DataSource::removeSeriesByLabel(QString label, bool update)
{
    if (data_series.contains(label))
    {
        data_series.remove(label);

        if (update)
        {
            emit dataChanged();
        }

        return true;
    }
    else
    {
        return false;
    }
}


void DataSource::removeAllSeries(bool update)
{
    data_series.clear();

    if (update)
    {
        emit dataChanged();
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
}


DataSourceManager::~DataSourceManager()
{
    removeAllSources(false);
}


DataSeriesPointer DataSourceManager::findSeries(QString source_label, QString series_label)
{
    auto source = getSourceByLabel(source_label);

    if (source.isNull()) return DataSeriesPointer(nullptr);

    return source->getSeriesByLabel(series_label);
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


DataSourcePointer DataSourceManager::getSourceByIndex(unsigned int idx)
{
    if (idx < sources.size()) return sources.at(idx);

    return DataSourcePointer(nullptr);
}


DataSourcePointer DataSourceManager::getSourceByLabel(QString label)
{
    for (auto src : sources)
    {
        if (src.isNull()) continue;

        if (src->getLabel() == label)
        {
            return src;
        }
    }

    return DataSourcePointer(nullptr);
}


bool DataSourceManager::addSource(DataSourcePointer source)
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

    connect(source.data(), &DataSource::dataChanged, this, &DataSourceManager::onDataChanged);

    emit sourcesChanged();

    return true;
}


/**
 * @brief DataSourceManager::addSource adds a DataSource with the given title
 * @param label is the label of the DataSource to add
 * @return true if the source was added, false if a DataSource with the given title already existed
 */
bool DataSourceManager::addSource(QString label, QString description)
{
    if (getSourceByLabel(label).isNull())
    {
        return addSource(new DataSource(label, description));
    }

    // Source with provided label already exists!
    return false;
}


/**
 * @brief DataSourceManager::removeSource removes the DataSource from this DataSourceManager
 * @param source is a shared pointer to the DataSource
 * @return true if the source was removed, else false
 */
bool DataSourceManager::removeSource(DataSourcePointer source)
{
    for (auto idx = 0; idx < sources.size(); idx++)
    {
        auto src = sources.at(idx);

        if (src == source)
        {
            sources.removeAt(idx);
            emit sourcesChanged();
            return true;
        }
    }

    return false;
}


/**
 * @brief DataSourceManager::removeSourceByIndex removes the DataSource at the specified index
 * @param idx - Index of the DataSource to remove
 * @param update - If true, emit a "sourcesChanged" event
 * @return true if the source was removed, else false (in the case of an invalid index)
 */
bool DataSourceManager::removeSourceByIndex(unsigned int idx, bool update)
{
    if (idx < sources.size())
    {
        sources.removeAt(idx);

        if (update)
        {
            emit sourcesChanged();
        }

        return true;
    }

    return false;
}


/**
 * @brief DataSourceManager::removeSourceByLabel - Remove the first DataSource which matches the specified label
 * @param label - QString label
 * @param update - If true, emit a "sourcesChanged" event
 * @return true if a source matching the provided label was found (and subsequentyly deleted)
 */
bool DataSourceManager::removeSourceByLabel(QString label, bool update)
{
    for (int idx = 0; idx < sources.size(); idx++)
    {
        auto src = sources.at(idx);

        if (!src.isNull() && src->getLabel() == label)
        {
            return removeSourceByIndex(idx, update);
        }
    }

    return false;
}


/**
 * @brief DataSourceManager::removeAllSources removes all DataSource objects from this DataSourceManager
 * @param update - if true, emit a "sourcesChanged" event
 */
void DataSourceManager::removeAllSources(bool update)
{
    while (sources.count() > 0)
    {
        removeSourceByIndex(0, update);
    }

    if (update)
    {
        emit sourcesChanged();
    }
}
