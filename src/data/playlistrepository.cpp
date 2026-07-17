#include "playlistrepository.h"
#include <QSqlQuery>
#include <QSqlError>

constexpr auto PlaylistColumns = R"(
    Playlists.id AS id,
    Playlists.owner_id AS owner_id,
    Playlists.name AS name
)";

Playlist PlaylistRepository::fromQuery(const QSqlQuery &query)
{
    Playlist result;
    result.ownerId = query.value("owner_id").value<ID>();
    result.name = query.value("name").toString();
    result.id = query.value("id").value<ID>();
    return result;
}

std::optional<ID> PlaylistRepository::insert(const Playlist &playlist)
{
    QSqlQuery query;
    query.prepare(R"(INSERT INTO Playlists(owner_id, name) VALUES(:owner_id, :name))");
    query.bindValue(":owner_id", playlist.ownerId);
    query.bindValue(":name", playlist.name);
    if (!query.exec())
    {
        qWarning() << "Failed to insert the playlist:" << query.lastError().text();
        return std::nullopt;
    }
    return query.lastInsertId().value<ID>();
}

std::optional<ID> PlaylistRepository::update(const Playlist &playlist)
{
    QSqlQuery query;
    query.prepare(R"(UPDATE Playlists SET owner_id = :owner_id, name = :name WHERE id = :id)");
    query.bindValue(":owner_id", playlist.ownerId);
    query.bindValue(":name", playlist.name);
    query.bindValue(":id", playlist.id);
    if (!query.exec())
    {
        qWarning() << "Failed to update the playlist:" << query.lastError().text();
        return std::nullopt;
    }
    return playlist.id;
}

bool PlaylistRepository::remove(ID playlistId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM Playlists WHERE id = :id");
    query.bindValue(":id", playlistId);
    if (!query.exec())
    {
        qWarning() << "Failed to delete the playlist:" << query.lastError().text();
    }
    return query.numRowsAffected() > 0;
}

std::optional<ID> PlaylistRepository::save(const Playlist &value)
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

std::optional<Playlist> PlaylistRepository::search(ID playlistId) const
{
    QSqlQuery query;
    query.prepare(QString("SELECT %1 FROM Playlists WHERE id = :id").arg(PlaylistColumns));
    query.bindValue(":id", playlistId);
    if (!query.exec())
    {
        qWarning() << "Failed to search the playlist:" << query.lastError().text();
    }
    else if (query.next())
    {
        return PlaylistRepository::fromQuery(query);
    }
    return std::nullopt;
}

std::optional<ID> PlaylistRepository::insertSong(ID playlistId, ID songId)
{
    QSqlQuery query;
    query.prepare(R"(INSERT INTO PlaylistSongs(song_id, playlist_id) VALUES (:song_id, :playlist_id))");
    query.bindValue(":playlist_id", playlistId);
    query.bindValue(":song_id", songId);
    if (!query.exec())
    {
        qWarning() << "Failed to add the song to the playlist:" << query.lastError().text();
        return std::nullopt;
    }
    else
    {
        return query.lastInsertId().value<ID>();
    }
}

bool PlaylistRepository::removeSong(ID playlistId, ID songId)
{
    QSqlQuery query;
    query.prepare(R"(DELETE FROM PlaylistSongs WHERE song_id = :song_id AND playlist_id = :playlist_id)");
    query.bindValue(":playlist_id", playlistId);
    query.bindValue(":song_id", songId);
    if (!query.exec())
    {
        qWarning() << "Failed to remove the song from the playlist:" << query.lastError().text();
        return false;
    }
    else
    {
        return true;
    }
}

PlaylistList PlaylistRepository::playlists(ID listenerId) const
{
    QSqlQuery query;
    query.prepare(QString("SELECT %1 FROM Playlists WHERE owner_id = :id").arg(PlaylistColumns));
    query.bindValue(":id", listenerId);
    if (!query.exec())
    {
        qWarning() << "Failed to fetch playlists:" << query.lastError().text();
        return {};
    }
    PlaylistList result;
    while (!query.next())
    {
        result.append(PlaylistRepository::fromQuery(query));
    }
    return result;
}

PlaylistRepository &PlaylistRepository::instance()
{
    static PlaylistRepository repository;
    return repository;
}
