#include "listenerrepository.h"
#include <QSqlError>
#include <QSqlQuery>

constexpr auto ListenerColumns = R"(
    Listeners.id AS id,
    Listeners.user_name AS user_name,
    Listeners.password AS password,
    Listeners.name AS name,
    Listeners.biography AS biography,
    Listeners.photo AS photo
)";

ListenerRepository& ListenerRepository::instance()
{
    static ListenerRepository repository;
    return repository;
}

bool ListenerRepository::isLiked(ID listenerId, ID songId) const
{
    QSqlQuery query;
    query.prepare(R"(
        SELECT
        song_id,
        listener_id
        FROM LikedSongs WHERE
        listener_id = :listener
        AND song_id = :song
    )");
    query.bindValue(":listener", listenerId);
    query.bindValue(":song", songId);
    if (!query.exec())
    {
        qWarning() << "Failed to see if the song is liked:" << query.lastError().text();
        return false;
    }
    return query.next();
}

bool ListenerRepository::setLiked(ID listenerId, ID songId, bool liked)
{
    QSqlQuery query;
    if (liked)
    {
        query.prepare(R"(
            INSERT OR IGNORE INTO LikedSongs(
                listener_id,
                song_id
            )
            VALUES(
                :listener,
                :song
            )
        )");
        query.bindValue(":listener", listenerId);
        query.bindValue(":song", songId);
        if (!query.exec())
        {
            qWarning() << "Failed to like song:" << query.lastError().text();
            return false;
        }
        else
        {
            return query.numRowsAffected() > 0;
        }
    }
    else
    {
        query.prepare(R"(
            DELETE FROM LikedSongs
            WHERE listener_id = :listener
            AND song_id = :song
        )");
        query.bindValue(":listener", listenerId);
        query.bindValue(":song", songId);
        if (!query.exec())
        {
            qWarning() << "Failed to unlike song:" << query.lastError().text();
            return false;
        }
        else
        {
            return query.numRowsAffected() > 0;
        }
    }
}

Account ListenerRepository::fromQuery(const QSqlQuery &query)
{
    Account result;
    result.id = query.value("id").value<ID>();
    result.userName = query.value("user_name").toString();
    result.password = query.value("password").toString();
    result.name = query.value("name").toString();
    result.biography = query.value("biography").toString();
    result.photo = Utility::Image::fromRawData(query.value("photo").toByteArray());
    result.role = Account::Listener;
    return result;
}

std::optional<ID> ListenerRepository::insert(const Account &value)
{
    QSqlQuery query;

    query.prepare(R"(
        INSERT INTO Listeners(
            user_name,
            password,
            name,
            biography,
            photo
        )
        VALUES(
            :user_name,
            :password,
            :name,
            :biography,
            :photo
        )
    )");
    query.bindValue(":user_name", value.userName);
    query.bindValue(":password", value.password);
    query.bindValue(":name", value.name);
    query.bindValue(":biography", value.biography);
    query.bindValue(":photo", Utility::Image::toRawData(value.photo));
    if (!query.exec())
    {
        qWarning() << "Failed to insert listener:" << query.lastError().text();
        return std::nullopt;
    }
    return query.lastInsertId().value<ID>();
}

bool ListenerRepository::update(const Account &value)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE Listeners
        SET
            user_name = :user_name,
            password = :password,
            name = :name,
            biography = :biography,
            photo = :photo
        WHERE id = :id
    )");
    query.bindValue(":id", value.id);
    query.bindValue(":user_name", value.userName);
    query.bindValue(":password", value.password);
    query.bindValue(":name", value.name);
    query.bindValue(":biography", value.biography);
    query.bindValue(":photo", Utility::Image::toRawData(value.photo));
    if (!query.exec())
    {
        qWarning() << "Failed to update listener:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

std::optional<ID> ListenerRepository::save(const Account &value)
{
    if (value.role != Account::Listener)
    {
        qWarning() << "Attempted to save a non-listener using the listener repository.";
        return std::nullopt;
    }
    if (value.id == InvalidId)
    {
        return insert(value);
    }
    if (update(value))
    {
        return value.id;
    }
    return std::nullopt;
}

std::optional<Account> ListenerRepository::search(ID value) const
{
    QSqlQuery query;
    query.prepare(QString("SELECT %1 FROM Listeners WHERE id = :id").arg(ListenerColumns));
    query.bindValue(":id", value);
    if (!query.exec())
    {
        qWarning() << "Failed to search listener:" << query.lastError().text();
        return std::nullopt;
    }
    if (!query.next())
    {
        return std::nullopt;
    }
    return fromQuery(query);
}

std::optional<Account> ListenerRepository::searchByUserName(const QString &userName) const
{
    QSqlQuery query;
    query.prepare(QString("SELECT %1 FROM Listeners WHERE user_name = :user_name").arg(ListenerColumns));
    query.bindValue(":user_name", userName);
    if (!query.exec())
    {
        qWarning() << "Failed to search listener:" << query.lastError().text();
        return std::nullopt;
    }
    if (!query.next())
    {
        return std::nullopt;
    }
    return fromQuery(query);
}

bool ListenerRepository::remove(ID value)
{
    QSqlQuery query;

    query.prepare(R"(DELETE FROM Listeners WHERE id = :id)");
    query.bindValue(":id", value);
    if (!query.exec())
    {
        qWarning() << "Failed to remove listener:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}
