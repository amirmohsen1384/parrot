#include "domain.h"
#include <QFile>
#include <QDataStream>

QImage Utility::Image::fromRawData(const QByteArray &data)
{
    QImage image;
    QDataStream stream(data);
    if((stream >> image).status() != QDataStream::Ok)
    {
        return QImage();
    }
    return image;
}

QByteArray Utility::Image::toRawData(const QImage &image)
{
    QByteArray result;
    QDataStream stream(&result, QFile::WriteOnly);
    if((stream << image).status() != QDataStream::Ok)
    {
        return QByteArray();
    }
    return result;
}
