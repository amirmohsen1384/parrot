#ifndef ACCOUNTSERVICE_H
#define ACCOUNTSERVICE_H

#include <QObject>
#include "data/domain.h"

class AccountService : public QObject
{
    Q_OBJECT
public:
    inline Account getAccount() const
    {
        return mainAccount;
    }

public slots:
    virtual bool updateAccount(const Account &info) = 0;

signals:
    void accountUpdated(const Account &info);

protected:
    Account mainAccount;
};

#endif // ACCOUNTSERVICE_H
