#ifndef DOMAIN_H
#define DOMAIN_H

#define INVALID_ID -1

#include <QString>
#include <QPixmap>
#include <QUrl>

using ID = qint64;

struct AccountData
{
    enum Role {
        Unknown,
        Listener,
        Artist
    };
    QImage photo;
    QString name;
    QString userName;
    QString password;
    QString biography;
    Role role = Role::Unknown;
};

struct Account
{
    ID id = INVALID_ID;
    AccountData data;
};

struct PlaylistData
{
    QString name;
    ID ownerId = INVALID_ID;
};

struct Playlist
{
    ID id = INVALID_ID;
    PlaylistData data;
};

using PlaylistList = QList<Playlist>;

struct AlbumData
{
    QString name;
    QImage photo;
    ID ownerId = INVALID_ID;
};

struct Album
{
    ID id = INVALID_ID;
    AlbumData data;
};

using AlbumList = QList<Album>;

struct SongData
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
    qint64 releasedYear = INVALID_ID;
    Genre genre = Genre::Unknown;
    ID artistId = INVALID_ID;
    ID albumId = INVALID_ID;
    QImage cover;
};

struct Song
{
    ID id = INVALID_ID;
    SongData data;
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
