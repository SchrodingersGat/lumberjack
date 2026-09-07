#include <QStyle>
#include <QWidget>

#include "flow_layout.hpp"


FlowLayout::FlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing)
{
    setContentsMargins(margin, margin, margin, margin);
}


FlowLayout::FlowLayout(int margin, int hSpacing, int vSpacing)
    : m_hSpace(hSpacing), m_vSpace(vSpacing)
{
    setContentsMargins(margin, margin, margin, margin);
}


FlowLayout::~FlowLayout()
{
    QLayoutItem *item;

    while ((item = takeAt(0)))
    {
        delete item;
    }
}


void FlowLayout::addItem(QLayoutItem *item)
{
    itemList.append(item);
}


int FlowLayout::horizontalSpacing() const
{
    if (m_hSpace >= 0)
    {
        return m_hSpace;
    }

    return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}


int FlowLayout::verticalSpacing() const
{
    if (m_vSpace >= 0)
    {
        return m_vSpace;
    }

    return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}


Qt::Orientations FlowLayout::expandingDirections() const
{
    return {};
}


bool FlowLayout::hasHeightForWidth() const
{
    return true;
}


int FlowLayout::heightForWidth(int width) const
{
    return doLayout(QRect(0, 0, width, 0), true);
}


int FlowLayout::count() const
{
    return itemList.size();
}


QLayoutItem *FlowLayout::itemAt(int index) const
{
    return itemList.value(index);
}


QLayoutItem *FlowLayout::takeAt(int index)
{
    if (index < 0 || index >= itemList.size())
    {
        return nullptr;
    }

    return itemList.takeAt(index);
}


QSize FlowLayout::minimumSize() const
{
    QSize size;

    for (const QLayoutItem *item : std::as_const(itemList))
    {
        size = size.expandedTo(item->minimumSize());
    }

    const QMargins margins = contentsMargins();
    size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());

    return size;
}


QSize FlowLayout::sizeHint() const
{
    return minimumSize();
}


void FlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}


int FlowLayout::doLayout(const QRect &rect, bool testOnly) const
{
    int left, top, right, bottom;

    getContentsMargins(&left, &top, &right, &bottom);

    QRect effectiveRect = rect.adjusted(left, top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;

    for (QLayoutItem *item : std::as_const(itemList))
    {
        QWidget *wid = item->widget();

        int spaceX = horizontalSpacing();

        if (spaceX == -1)
        {
            spaceX = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
        }

        int spaceY = verticalSpacing();

        if (spaceY == -1)
        {
            spaceY = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
        }

        int nextX = x + item->sizeHint().width() + spaceX;

        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0)
        {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }

        if (!testOnly)
        {
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
        }

        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }

    return y + lineHeight - rect.y() + bottom;
}


int FlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject *parent = this->parent();

    if (!parent)
    {
        return -1;
    }

    if (parent->isWidgetType())
    {
        auto pw = static_cast<QWidget *>(parent);
        return pw->style()->pixelMetric(pm, nullptr, pw);
    }

    return static_cast<QLayout *>(parent)->spacing();
}
