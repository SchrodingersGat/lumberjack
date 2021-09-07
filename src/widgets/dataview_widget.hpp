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

signals:
    void fileDropped(QString filename);

protected:
    Ui::dataview ui;

    virtual void dropEvent(QDropEvent *event) override;
    virtual void dragEnterEvent(QDragEnterEvent *event) override;

//    virtual void dragMoveEvent(QDragMoveEvent *event) override;
//    virtual void dragLeaveEvent(QDragLeaveEvent *event) override;
};


#endif // DATAVIEW_WIDGET_HPP
