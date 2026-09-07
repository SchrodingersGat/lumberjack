#include "stats_filter_proxy_model.hpp"


StatsFilterProxyModel::StatsFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}


void StatsFilterProxyModel::setNameFilter(const QString &text)
{
    nameFilterText = text;

    invalidateFilter();
}


void StatsFilterProxyModel::setValueFilter(int column, CompareOp op, double value)
{
    valueFilterEnabled = true;
    valueFilterColumn = column;
    valueFilterOp = op;
    valueFilterThreshold = value;

    invalidateFilter();
}


void StatsFilterProxyModel::clearValueFilter()
{
    valueFilterEnabled = false;

    invalidateFilter();
}


bool StatsFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    auto model = sourceModel();

    if (!model) return true;

    if (!nameFilterText.isEmpty())
    {
        auto nameIndex = model->index(sourceRow, 0, sourceParent);
        QString name = model->data(nameIndex, Qt::DisplayRole).toString();

        if (!name.contains(nameFilterText, Qt::CaseInsensitive))
        {
            return false;
        }
    }

    if (valueFilterEnabled)
    {
        auto valueIndex = model->index(sourceRow, valueFilterColumn, sourceParent);
        QVariant data = model->data(valueIndex, StatsRoles::ValueRole);

        // Rows with no numeric value for this column (e.g. no samples fall within
        // the current time interval) never satisfy a value filter
        if (!data.isValid()) return false;

        double value = data.toDouble();

        switch (valueFilterOp)
        {
        case GreaterThan:    if (!(value > valueFilterThreshold))  return false; break;
        case GreaterOrEqual: if (!(value >= valueFilterThreshold)) return false; break;
        case LessThan:       if (!(value < valueFilterThreshold))  return false; break;
        case LessOrEqual:    if (!(value <= valueFilterThreshold)) return false; break;
        case Equal:          if (value != valueFilterThreshold)    return false; break;
        }
    }

    return true;
}
