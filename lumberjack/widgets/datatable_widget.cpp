#include "datatable_widget.hpp"


DataSeriesTableModel::DataSeriesTableModel(QSharedPointer<DataSeries> series, QObject* parent) : QAbstractTableModel(parent)
{
    this->series = series;
}


int DataSeriesTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (series.isNull())
    {
        return 0;
    }
    else
    {
        return series->size();
    }
}


int DataSeriesTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 2;
}


QVariant DataSeriesTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        int row = index.row();
        int col = index.column();

        // Out of bounds
        if (series.isNull() || row < 0 || row >= series->size())
        {
            return QVariant();
        }

        DataPoint point = series->getDataPoint(row);

        switch (col)
        {
        case 0:
            return point.timestamp;
        case 1:
            return point.value;
        default:
            return "Invalid index: " + QString::number(col);
        }
    }
    else
    {
        return QVariant();
    }
}


DataSeriesTableView::DataSeriesTableView(QSharedPointer<DataSeries> series, QWidget *parent) :
    QTableView(parent),
    model(series)
{
    setModel(&model);
}
