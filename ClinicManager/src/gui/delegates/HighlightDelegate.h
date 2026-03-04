#pragma once

#include <QStyledItemDelegate>
#include <QPainter>

// Highlights rows based on patient priority (already set via BackgroundRole in the model,
// but this delegate adds a left-side colored indicator bar for extra visual feedback).
class HighlightDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit HighlightDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
};
