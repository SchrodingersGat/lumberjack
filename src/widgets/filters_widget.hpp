#ifndef FILTERS_WIDGET_H
#define FILTERS_WIDGET_H

#include <QWidget>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

#include "data_series.hpp"

#include "ui_filters_widget.h"


class FiltersWidget : public QWidget
{
    Q_OBJECT

public:
    FiltersWidget(QWidget *parent = nullptr);

public slots:
    void addSeries(DataSeriesPointer series);
    void loadPlugins(void);

    void clearItems(void);
    void applyFilter(void);

signals:

protected:
    Ui::filtersview ui;

    QList<DataSeriesPointer> seriesList;

    // Drag-n-drop actions
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dragMoveEvent(QDragMoveEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

    void refresh(void);
    void updateSeriesList(void);
};

#endif // FILTERS_WIDGET_H
