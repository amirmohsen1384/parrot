#ifndef ARTISTREPOSITORY_H
#define ARTISTREPOSITORY_H

#include "accountrepository.h"

class ArtistRepository : public AccountRepository
{
private:
    ArtistRepository() = default;
    Q_DISABLE_COPY_MOVE(ArtistRepository)

public:
    static ArtistRepository& instance();

    std::optional<Account> searchByUserName(const QString &userName) const override;
    std::optional<Account> search(ID value) const override;
    std::optional<ID> save(const Account &value) override;
    bool remove(ID value) override;

protected:
    virtual std::optional<ID> insert(const AccountData &value) override;
    virtual Account fromQuery(const QSqlQuery &query) const override;
    virtual bool update(const Account &value) override;
};

#endif // ARTISTREPOSITORY_H
