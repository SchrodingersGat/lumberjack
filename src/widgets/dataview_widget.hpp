#ifndef DATAVIEW_WIDGET_HPP
#define DATAVIEW_WIDGET_HPP

#include <QWidget>

#include "ui_dataview_widget.h"

class DataviewWidget : public QWidget
{
    Q_OBJECT

public:
    DataviewWidget(QWidget *parent = nullptr);

    DataViewTree *getTree() { return ui.treeWidget; }

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
};


#endif // DATAVIEW_WIDGET_HPP
