#include "songrepository.h"
#include <QSqlQuery>
#include <QSqlError>

constexpr auto SongColumns = R"(
    Songs.id AS id,
    Songs.file_name AS file_name,
    Songs.released_year AS released_year,
    Songs.genre AS genre,
    Songs.name AS name,
    Songs.artist_id AS artist_id,
    Songs.album_id AS album_id,
    Songs.cover AS cover
)";

std::optional<ID> SongRepository::insert(const Song &value)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO Songs(
            file_name,
            released_year,
            genre,
            name,
            artist_id,
            album_id,
            cover
        )
        VALUES(
            :file_name,
            :released_year,
            :genre,
            :name,
            :artist_id,
            :album_id,
            :cover
        )
    )");
    query.bindValue(":file_name", value.fileName.toLocalFile());
    query.bindValue(":released_year", value.releasedYear);
    query.bindValue(":genre", static_cast<int>(value.genre));
    query.bindValue(":name", value.name);
    query.bindValue(":artist_id", value.artistId);
    if (value.albumId == InvalidId)
    {
        query.bindValue(":album_id", QVariant(QMetaType::fromType<ID>()));
    }
    else
    {
        query.bindValue(":album_id", value.albumId);
    }
    query.bindValue(":cover", Utility::Image::toRawData(value.cover));

    if (!query.exec())
    {
        qWarning() << "Failed to insert song:" << query.lastError().text();
        return std::nullopt;
    }

    return query.lastInsertId().value<ID>();
}

Song SongRepository::fromQuery(const QSqlQuery &query)
{
    Song result;
    result.id = query.value("id").value<ID>();
    result.name = query.value("name").toString();
    result.fileName = QUrl::fromLocalFile(query.value("file_name").toString());
    result.genre = static_cast<Song::Genre>(query.value("genre").toInt());
    result.releasedYear = query.value("released_year").toLongLong();
    result.cover = Utility::Image::fromRawData(query.value("cover").toByteArray());
    result.albumId = query.value("album_id").value<ID>();
    result.artistId = query.value("artist_id").value<ID>();
    return result;
}

std::optional<ID> SongRepository::update(const Song &value)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE Songs
        SET
            file_name = :file_name,
            released_year = :released_year,
            genre = :genre,
            name = :name,
            artist_id = :artist_id,
            album_id = :album_id,
            cover = :cover
        WHERE id = :id
    )");
    query.bindValue(":id", value.id);
    query.bindValue(":file_name", value.fileName.toLocalFile());
    query.bindValue(":released_year", value.releasedYear);
    query.bindValue(":genre", static_cast<int>(value.genre));
    query.bindValue(":name", value.name);
    query.bindValue(":artist_id", value.artistId);
    if (value.albumId == InvalidId)
    {
        query.bindValue(":album_id", QVariant(QMetaType::fromType<ID>()));
    }
    else
    {
        query.bindValue(":album_id", value.albumId);
    }

    query.bindValue(":cover", Utility::Image::toRawData(value.cover));
    if (!query.exec())
    {
        qWarning() << "Failed to update song:" << query.lastError().text();
        return std::nullopt;
    }

    return value.id;
}

std::optional<ID> SongRepository::save(const Song &value)
{
    if (value.id == InvalidId)
    {
        return insert(value);
    }
    else
    {
        return update(value);
    }
}

std::optional<Song> SongRepository::search(ID value) const
{
    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT %1 FROM Songs WHERE id = :id").arg(SongColumns));
    query.bindValue(":id", value);
    if (!query.exec())
    {
        qWarning() << "Failed to execute the search query:" << query.lastError().text();
        return std::nullopt;
    }
    else if (!query.next())
    {
        return std::nullopt;
    }
    return SongRepository::fromQuery(query);
}

bool SongRepository::remove(ID value)
{
    QSqlQuery query;
    query.prepare("DELETE FROM Songs WHERE id = :id");
    query.bindValue(":id", value);
    if (!query.exec())
    {
        qWarning() << "Failed to execute the query:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

SongRepository &SongRepository::instance()
{
    static SongRepository repository;
    return repository;
}

SongList SongRepository::byLikedSongs(ID accountId) const
{
    QSqlQuery query;
    query.prepare(QStringLiteral(R"(
        SELECT
        %1
        FROM Songs
        JOIN LikedSongs ON Songs.id = LikedSongs.song_id
        WHERE LikedSongs.listener_id = :id
    )").arg(SongColumns));
    query.bindValue(":id", accountId);
    if (!query.exec())
    {
        qWarning() << "Failed to execute the query to get the liked songs:" << query.lastError().text();
        return {};
    }
    SongList result;
    while (query.next())
    {
        result.append(SongRepository::fromQuery(query));
    }
    return result;
}

SongList SongRepository::byPlaylist(ID playlistId) const
{
    QSqlQuery query;
    query.prepare(QStringLiteral(R"(
        SELECT
        %1
        FROM Songs
        JOIN PlaylistSongs ON Songs.id = PlaylistSongs.song_id
        WHERE PlaylistSongs.playlist_id = :id
    )").arg(SongColumns));
    query.bindValue(":id", playlistId);
    if (!query.exec())
    {
        qWarning() << "Failed to execute the query to get the playlist songs:" << query.lastError().text();
        return {};
    }
    SongList result;
    while (query.next())
    {
        result.append(SongRepository::fromQuery(query));
    }
    return result;
}

SongList SongRepository::singleSongs(ID artistId) const
{
    QSqlQuery query;
    query.prepare(QStringLiteral(R"(
        SELECT
        %1
        FROM Songs
        WHERE Songs.album_id IS NULL AND Songs.artist_id = :id
    )").arg(SongColumns));
    query.bindValue(":id", artistId);
    if (!query.exec())
    {
        qWarning() << "Failed to execute the query to get the songs:" << query.lastError().text();
        return {};
    }
    SongList result;
    while (query.next())
    {
        result.append(SongRepository::fromQuery(query));
    }
    return result;
}

SongList SongRepository::byArtist(ID artistId) const
{
    QSqlQuery query;
    query.prepare(QStringLiteral(R"(
        SELECT
        %1
        FROM Songs
        WHERE Songs.artist_id = :id
    )").arg(SongColumns));
    query.bindValue(":id", artistId);
    if (!query.exec())
    {
        qWarning() << "Failed to execute the query to get the songs:" << query.lastError().text();
        return {};
    }
    SongList result;
    while (query.next())
    {
        result.append(SongRepository::fromQuery(query));
    }
    return result;
}

SongList SongRepository::byAlbum(ID albumId) const
{
    QSqlQuery query;
    query.prepare(QStringLiteral(R"(
        SELECT
        %1
        FROM Songs
        WHERE Songs.album_id = :id
    )").arg(SongColumns));
    query.bindValue(":id", albumId);
    if (!query.exec())
    {
        qWarning() << "Failed to execute the query to get the songs:" << query.lastError().text();
        return {};
    }
    SongList result;
    while (query.next())
    {
        result.append(SongRepository::fromQuery(query));
    }
    return result;
}
