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

    void onItemDoubleClicked(QTreeWidgetItem *item, int col);

protected:
    virtual void startDrag(Qt::DropActions supported_actions) override;

    void setupTree();
};


#endif // DATAVIEW_TREE_HPP
