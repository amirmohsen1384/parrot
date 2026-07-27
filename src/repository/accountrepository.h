#ifndef ACCOUNTREPOSITORY_H
#define ACCOUNTREPOSITORY_H

#include "abstractrepository.h"

class AccountRepository : public AbstractRepository<Account>
{
public:
    virtual std::optional<Account> searchByUserName(const QString &userName) const = 0;

protected:
    virtual std::optional<ID> insert(const AccountData &value);
    virtual Account fromQuery(const QSqlQuery &query) const;
    virtual bool update(const Account &value);
};

#endif // ACCOUNTREPOSITORY_H
