#ifndef DATAVIEW_TREE_HPP
#define DATAVIEW_TREE_HPP

#include <QTreeWidget>

#include "data_series.hpp"

class DataViewTree : public QTreeWidget
{
    Q_OBJECT

public:
    DataViewTree(QWidget *parent = nullptr);
    virtual ~DataViewTree();

public slots:
    int refresh(QString filters=QString());

    void onItemDoubleClicked(QTreeWidgetItem *item, int col);
    void onContextMenu(const QPoint &pos);

signals:
    void onSeriesRemoved(DataSeriesPointer series);

protected:
    virtual void startDrag(Qt::DropActions supported_actions) override;

    void setupTree();
    void editDataSeries(DataSeriesPointer series);

    QString filterString;
};


#endif // DATAVIEW_TREE_HPP
