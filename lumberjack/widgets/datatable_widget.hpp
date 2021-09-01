#ifndef DATATABLE_WIDGET_HPP
#define DATATABLE_WIDGET_HPP

#include <qtableview.h>
#include <qwidget.h>

#include "data_series.hpp"


class DataSeriesTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    DataSeriesTableModel(QSharedPointer<DataSeries> series, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    QSharedPointer<DataSeries> series;
};


class DataSeriesTableView : public QTableView
{
    Q_OBJECT

public:
    DataSeriesTableView(QSharedPointer<DataSeries> series, QWidget *parent = nullptr);
    virtual ~DataSeriesTableView();

protected:
    DataSeriesTableModel model;
};


#endif // DATATABLE_WIDGET_HPP
