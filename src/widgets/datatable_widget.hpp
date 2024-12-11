#ifndef DATATABLE_WIDGET_HPP
#define DATATABLE_WIDGET_HPP

#include <qtableview.h>
#include <qwidget.h>

#include "data_series.hpp"


class DataSeriesTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    DataSeriesTableModel(DataSeriesPointer series, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    DataSeriesPointer series;
};


class DataSeriesTableView : public QTableView
{
    Q_OBJECT

public:
    DataSeriesTableView(DataSeriesPointer series, QWidget *parent = nullptr);
    virtual ~DataSeriesTableView();

protected:
    DataSeriesTableModel model;
};


#endif // DATATABLE_WIDGET_HPP
