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

Account ArtistRepository::fromQuery(const QSqlQuery &query) const
{
    auto result = AccountRepository::fromQuery(query);
    result.data.role = AccountData::Artist;
    return result;
}

std::optional<ID> ArtistRepository::insert(const AccountData &value)
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
    query.bindValue(":photo", Utility::Image::toRawData(value.photo));
    query.bindValue(":biography", value.biography);
    query.bindValue(":user_name", value.userName);
    query.bindValue(":password", value.password);
    query.bindValue(":name", value.name);
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
    query.bindValue(":name", value.data.name);
    query.bindValue(":user_name", value.data.userName);
    query.bindValue(":password", value.data.password);
    query.bindValue(":biography", value.data.biography);
    query.bindValue(":photo", Utility::Image::toRawData(value.data.photo));
    if (!query.exec())
    {
        qWarning() << "Failed to update artist:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

std::optional<ID> ArtistRepository::save(const Account &value)
{
    if (value.data.role != AccountData::Artist)
    {
        qWarning() << "Cannot save a non-artist account using the artist repository.";
        return std::nullopt;
    }
    if (value.id == INVALID_ID)
    {
        return insert(value.data);
    }
    else
    {
        return update(value);
    }
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
