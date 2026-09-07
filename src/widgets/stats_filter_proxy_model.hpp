#ifndef STATS_FILTER_PROXY_MODEL_HPP
#define STATS_FILTER_PROXY_MODEL_HPP

#include <QSortFilterProxyModel>


namespace StatsRoles
{
    // Backs numeric sorting/filtering with the raw value, independent of how
    // Qt::DisplayRole happens to format the cell (e.g. "-" for no-data cells)
    constexpr int ValueRole = Qt::UserRole + 1;
}


/**
 * @brief The StatsFilterProxyModel class filters the rows of the statistics
 * table by series name (substring match) and/or a numeric threshold on a
 * single column (e.g. "Max > 0").
 */
class StatsFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    enum CompareOp
    {
        GreaterThan,
        GreaterOrEqual,
        LessThan,
        LessOrEqual,
        Equal,
    };

    explicit StatsFilterProxyModel(QObject *parent = nullptr);

    void setNameFilter(const QString &text);

    void setValueFilter(int column, CompareOp op, double value);
    void clearValueFilter();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString nameFilterText;

    bool valueFilterEnabled = false;
    int valueFilterColumn = 0;
    CompareOp valueFilterOp = GreaterThan;
    double valueFilterThreshold = 0.0;
};

#endif // STATS_FILTER_PROXY_MODEL_HPP
