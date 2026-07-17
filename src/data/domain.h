#ifndef DOMAIN_H
#define DOMAIN_H

#include <QString>
#include <QPixmap>
#include <QUrl>

using ID = qint64;
constexpr int InvalidId = 0;

struct Account
{
    enum Role {Unknown, Listener, Artist};
    QString name;
    QString userName;
    QString biography;
    ID id = InvalidId;
    Role role = Role::Unknown;
    QString password;
    QImage photo;
};

struct Playlist
{
    ID id = 0;
    QString name;
    ID ownerId = 0;
};

using PlaylistList = QList<Playlist>;

struct Album
{
    QString name;
    QImage photo;
    ID id = InvalidId;
    ID ownerId = InvalidId;
};

using AlbumList = QList<Album>;

struct Song
{
    enum class Genre
    {
        Unknown,
        Rap,
        Jazz,
        Pop,
        Rock,
        Classic,
        Folk,
        Country
    };
    QString name;
    QUrl fileName;
    qint64 releasedYear = InvalidId;
    Genre genre = Genre::Unknown;
    ID id = InvalidId;
    ID artistId = InvalidId;
    ID albumId = InvalidId;
    QImage cover;
};

using SongList = QList<Song>;

#include <QImage>

namespace Utility
{
    namespace Image
    {
        QImage fromRawData(const QByteArray &data);
        QByteArray toRawData(const QImage &image);
    }
}

#endif // DOMAIN_H
