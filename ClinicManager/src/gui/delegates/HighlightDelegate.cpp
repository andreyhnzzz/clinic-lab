#include "HighlightDelegate.h"

#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QApplication>
#include <QModelIndex>

HighlightDelegate::HighlightDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

void HighlightDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    // Draw a 4-pixel priority indicator on the left edge of column 0
    if (index.column() != 0) return;

    QVariant bg = index.data(Qt::BackgroundRole);
    if (!bg.isValid()) return;

    QColor color = bg.value<QColor>();
    painter->save();
    painter->fillRect(QRect(option.rect.left(), option.rect.top(), 4, option.rect.height()),
                      color.darker(130));
    painter->restore();
}
