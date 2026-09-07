#ifndef FLOW_LAYOUT_HPP
#define FLOW_LAYOUT_HPP

#include <QLayout>
#include <QList>
#include <QRect>
#include <QStyle>


/**
 * @brief The FlowLayout class arranges its child widgets left-to-right,
 * wrapping onto additional rows as needed to fit the available width -
 * similar to how inline text wraps. Used in place of a plain QHBoxLayout
 * wherever a row of controls should stay usable in a narrow panel instead
 * of forcing the panel to grow (or clipping/scrolling) to fit them all on
 * one line.
 */
class FlowLayout : public QLayout
{
public:
    explicit FlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    explicit FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
    ~FlowLayout() override;

    void addItem(QLayoutItem *item) override;

    int horizontalSpacing() const;
    int verticalSpacing() const;

    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;

    int count() const override;
    QLayoutItem *itemAt(int index) const override;
    QLayoutItem *takeAt(int index) override;

    QSize minimumSize() const override;
    QSize sizeHint() const override;
    void setGeometry(const QRect &rect) override;

private:
    int doLayout(const QRect &rect, bool testOnly) const;
    int smartSpacing(QStyle::PixelMetric pm) const;

    QList<QLayoutItem *> itemList;
    int m_hSpace;
    int m_vSpace;
};

#endif // FLOW_LAYOUT_HPP
