#ifndef ACCOUNTREPOSITORY_H
#define ACCOUNTREPOSITORY_H

#include "abstractrepository.h"

class AccountRepository : public AbstractRepository<Account>
{
public:
    virtual std::optional<Account> searchByUserName(const QString &userName) const = 0;
};

#endif // ACCOUNTREPOSITORY_H
