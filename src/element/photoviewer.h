#ifndef PHOTOVIEWER_H
#define PHOTOVIEWER_H

#include <QImage>
#include <QWidget>

class PhotoViewer : public QWidget
{
protected:
    virtual QSize sizeHint() const override;
    virtual void paintEvent(QPaintEvent *event) override;

public:
    explicit PhotoViewer(QWidget *parent = nullptr);
    explicit PhotoViewer(const QImage &image, QWidget *parent = nullptr);

private:
    QImage photo;
};

#endif // PHOTOVIEWER_H
