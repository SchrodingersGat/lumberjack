#ifndef DATAVIEW_WIDGET_HPP
#define DATAVIEW_WIDGET_HPP

#include <QWidget>

#include "data_source.hpp"

#include "ui_dataview_widget.h"

class DataviewWidget : public QWidget
{
    Q_OBJECT

public:
    DataviewWidget(QWidget *parent = nullptr);

public slots:
    void refresh();

    void filterTextUpdated(QString text);
    void clearFilter(void);

protected:
    Ui::dataview ui;

    void initTree(void);
};


#endif // DATAVIEW_WIDGET_HPP
