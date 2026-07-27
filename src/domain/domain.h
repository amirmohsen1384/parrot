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
    Account() = default;
    Account(const AccountData &info) : data(info) {}
};

struct PlaylistData
{
    QString name;
};

struct Playlist
{
    ID ownerId = INVALID_ID;
    ID id = INVALID_ID;
    PlaylistData data;
    Playlist() = default;
    Playlist(const PlaylistData &info) : data(info) {}
};

using PlaylistList = QList<Playlist>;

struct AlbumData
{
    QString name;
    QImage photo;
};

struct Album
{
    ID ownerId = INVALID_ID;
    ID id = INVALID_ID;
    AlbumData data;
    Album() = default;
    Album(const AlbumData &info) : data(info) {}
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
    QImage cover;
    QString name;
    QUrl fileName;
    qint64 releasedYear = 0;
    Genre genre = Genre::Unknown;
};

struct Song
{
    ID ownerId = INVALID_ID;
    ID albumId = INVALID_ID;
    ID id = INVALID_ID;
    SongData data;
    Song() = default;
    Song(const SongData &info) : data(info) {}
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
