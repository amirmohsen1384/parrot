#include "accountrepository.h"
#include <QSqlQuery>

Account AccountRepository::fromQuery(const QSqlQuery &query) const
{
    Account result;
    result.id = query.value("id").value<ID>();
    result.data.name = query.value("name").toString();
    result.data.password = query.value("password").toString();
    result.data.userName = query.value("user_name").toString();
    result.data.biography = query.value("biography").toString();
    result.data.photo = Utility::Image::fromRawData(query.value("photo").toByteArray());
    return result;
}
