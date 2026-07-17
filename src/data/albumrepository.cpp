#include "albumrepository.h"
#include <QSqlQuery>
#include <QSqlError>

constexpr auto AlbumColumns = R"(
    Albums.id AS id,
    Albums.name AS name,
    Albums.artist_id AS artist_id,
    Albums.cover AS cover
)";

AlbumRepository &AlbumRepository::instance()
{
    static AlbumRepository repository;
    return repository;
}

std::optional<ID> AlbumRepository::insert(const Album &value)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO Albums(
            name,
            artist_id,
            cover
        )
        VALUES(
            :name,
            :artist_id,
            :cover
        )
    )");
    query.bindValue(":name", value.name);
    query.bindValue(":artist_id", value.ownerId);
    query.bindValue(":cover", Utility::Image::toRawData(value.photo));
    if (!query.exec())
    {
        qWarning() << "Failed to insert album:" << query.lastError().text();
        return std::nullopt;
    }

    return query.lastInsertId().value<ID>();
}

bool AlbumRepository::update(const Album &value)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE Albums
        SET
            name = :name,
            artist_id = :artist_id,
            cover = :cover
        WHERE id = :id
    )");
    query.bindValue(":id", value.id);
    query.bindValue(":name", value.name);
    query.bindValue(":artist_id", value.ownerId);
    query.bindValue(":cover", Utility::Image::toRawData(value.photo));
    if (!query.exec())
    {
        qWarning() << "Failed to update album:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

std::optional<ID> AlbumRepository::save(const Album &value)
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

Album AlbumRepository::fromQuery(const QSqlQuery &query)
{
    Album result;
    result.id = query.value("id").value<ID>();
    result.name = query.value("name").toString();
    result.ownerId = query.value("artist_id").value<ID>();
    result.photo = Utility::Image::fromRawData(query.value("cover").toByteArray());
    return result;
}

std::optional<Album> AlbumRepository::search(ID value) const
{
    QSqlQuery query;
    query.prepare(QString("SELECT %1 FROM Albums WHERE id = :id").arg(AlbumColumns));
    query.bindValue(":id", value);
    if (!query.exec())
    {
        qWarning() << "Failed to search album:" << query.lastError().text();
        return std::nullopt;
    }
    else if (!query.next())
    {
        return std::nullopt;
    }
    return fromQuery(query);
}

bool AlbumRepository::remove(ID value)
{
    QSqlQuery query;
    query.prepare(R"(DELETE FROM Albums WHERE id = :id)");
    query.bindValue(":id", value);
    if (!query.exec())
    {
        qWarning() << "Failed to remove album:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() == 1;
}

AlbumList AlbumRepository::albums(ID artistId) const
{
    QSqlQuery query;
    query.prepare(QString(R"(SELECT %1 FROM Albums WHERE artist_id = :artist_id)").arg(AlbumColumns));
    query.bindValue(":artist_id", artistId);
    if (!query.exec())
    {
        qWarning() << "Failed to fetch albums:" << query.lastError().text();
        return {};
    }
    AlbumList result;
    while (query.next())
    {
        result.append(fromQuery(query));
    }
    return result;
}
