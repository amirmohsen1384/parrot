#include "artistrepository.h"
#include <QSqlError>
#include <QSqlQuery>

constexpr auto ArtistColumns = R"(
    Artists.id AS id,
    Artists.user_name AS user_name,
    Artists.password AS password,
    Artists.name AS name,
    Artists.biography AS biography,
    Artists.photo AS photo
)";

ArtistRepository &ArtistRepository::instance()
{
    static ArtistRepository repository;
    return repository;
}

Account ArtistRepository::fromQuery(const QSqlQuery &query)
{
    Account result;
    result.id = query.value("id").value<ID>();
    result.userName = query.value("user_name").toString();
    result.password = query.value("password").toString();
    result.name = query.value("name").toString();
    result.biography = query.value("biography").toString();
    result.photo = Utility::Image::fromRawData(query.value("photo").toByteArray());
    result.role = Account::Artist;
    return result;
}

std::optional<ID> ArtistRepository::insert(const Account &value)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO Artists(
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
        qWarning() << "Failed to insert artist:" << query.lastError().text();
        return std::nullopt;
    }
    return query.lastInsertId().value<ID>();
}

bool ArtistRepository::update(const Account &value)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE Artists
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
        qWarning() << "Failed to update artist:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

std::optional<ID> ArtistRepository::save(const Account &value)
{
    if (value.role != Account::Artist)
    {
        qWarning() << "Cannot save a non-artist account using the artist repository.";
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

std::optional<Account> ArtistRepository::search(ID value) const
{
    QSqlQuery query;
    query.prepare(QString("SELECT %1 FROM Artists WHERE id = :id").arg(ArtistColumns));
    query.bindValue(":id", value);
    if (!query.exec())
    {
        qWarning() << "Failed to search artist:" << query.lastError().text();
        return std::nullopt;
    }
    if (!query.next())
    {
        return std::nullopt;
    }
    return fromQuery(query);
}

std::optional<Account> ArtistRepository::searchByUserName(const QString &userName) const
{
    QSqlQuery query;
    query.prepare(QString("SELECT %1 FROM Artists WHERE user_name = :user_name").arg(ArtistColumns));
    query.bindValue(":user_name", userName);
    if (!query.exec())
    {
        qWarning() << "Failed to search artist:" << query.lastError().text();
        return std::nullopt;
    }
    if (!query.next())
    {
        return std::nullopt;
    }

    return fromQuery(query);
}

bool ArtistRepository::remove(ID value)
{
    QSqlQuery query;
    query.prepare(R"(DELETE FROM Artists WHERE id = :id)");
    query.bindValue(":id", value);
    if (!query.exec())
    {
        qWarning() << "Failed to remove artist:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}
