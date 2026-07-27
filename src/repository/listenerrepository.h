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

    bool isLiked(ID listenerId, ID songId) const;
    bool setLiked(ID listenerId, ID songId, bool liked = true);

protected:
    virtual std::optional<ID> insert(const AccountData &value) override;
    virtual Account fromQuery(const QSqlQuery &query) const override;
    virtual bool update(const Account &value) override;
};

#endif
