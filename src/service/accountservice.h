#ifndef ACCOUNTSERVICE_H
#define ACCOUNTSERVICE_H

#include "abstractservice.h"

class AccountService : public AbstractService
{
    Q_OBJECT
public:
    AccountService(QObject *parent = nullptr);
    Account account() const;

public slots:
    virtual bool updateAccount(const AccountData &info);

public:
    virtual bool isValid() const override;
    static std::optional<ID> createAccount(const AccountData &data);

signals:
    void accountUpdated(const AccountData &info);

protected:
    Account mainAccount;
};

#endif // ACCOUNTSERVICE_H
