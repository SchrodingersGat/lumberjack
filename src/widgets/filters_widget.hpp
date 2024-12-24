#ifndef FILTERS_WIDGET_H
#define FILTERS_WIDGET_H

#include <QWidget>

#include "ui_filters_widget.h"


class FiltersWidget : public QWidget
{
    Q_OBJECT

public:
    FiltersWidget(QWidget *parent = nullptr);

public slots:

signals:

protected:
    Ui::filtersview ui;
};

#endif // FILTERS_WIDGET_H
