#include "photoviewer.h"
#include <QPaintEvent>
#include <QPainter>

QSize PhotoViewer::sizeHint() const
{
    return QSize(96, 96);
}

static void renderBorder(QPainter *painter, const QRectF &region, int width)
{
    painter->save();
    auto gradient = QLinearGradient { QPointF(0.5, 0.0), QPointF(0.5, 1.0) };
    gradient.setColorAt(0.2, Qt::blue);
    gradient.setColorAt(0.8, Qt::cyan);
    painter->setPen(QPen(gradient, width));
    painter->drawEllipse(region);
    painter->restore();
}

void PhotoViewer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const auto region = QRegion { event->rect(), QRegion::Ellipse };
    renderBorder(&painter, event->rect(), 5);
    painter.setClipRegion(region);
    painter.drawImage(event->rect(), photo);
}

PhotoViewer::PhotoViewer(QWidget *parent) : QWidget(parent) {}

PhotoViewer::PhotoViewer(const QImage &image, QWidget *parent) : photo(image), QWidget(parent) {}
