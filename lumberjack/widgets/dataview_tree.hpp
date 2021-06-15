#ifndef DATAVIEW_TREE_HPP
#define DATAVIEW_TREE_HPP


#include <QTreeWidget>


class DataViewTree : public QTreeWidget
{
    Q_OBJECT

public:
    DataViewTree(QWidget *parent = nullptr);
    virtual ~DataViewTree();

public slots:
    int refresh(QString filters=QString());

protected:
    virtual void startDrag(Qt::DropActions supported_actions) override;
};


#endif // DATAVIEW_TREE_HPP
