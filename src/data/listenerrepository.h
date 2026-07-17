#ifndef LISTENERREPOSITORY_H
#define LISTENERREPOSITORY_H

#include "accountrepository.h"

class QSqlQuery;

class ListenerRepository : public AccountRepository
{
private:
    ListenerRepository() = default;
    Q_DISABLE_COPY_MOVE(ListenerRepository)

public:
    std::optional<Account> searchByUserName(const QString &userName) const override;
    std::optional<Account> search(ID value) const override;
    std::optional<ID> save(const Account &value) override;
    bool remove(ID value) override;

    static ListenerRepository& instance();

private:
    static Account fromQuery(const QSqlQuery &query);
    std::optional<ID> insert(const Account &value);
    bool update(const Account &value);
};

#endif
