#include <QDebug>
#include <optional>
#include "accountservice.h"
#include "data/artistrepository.h"
#include "data/listenerrepository.h"

AccountService::AccountService(const Account &data, QObject *parent) : QObject{parent}
{
    if (data.id != InvalidId)
    {
        switch (data.role)
        {
        case Account::Artist:
        {
            auto result = ArtistRepository::instance().search(data.id);
            if (!result.has_value())
            {
                if(!ArtistRepository::instance().save(data))
                {

                }
            }
            else
            {
                mainAccount = data;
                canBeUsed = true;
            }
            break;
        }
        case Account::Listener:
        {
            canBeUsed = true;
            break;
        }
        default:
        {
            break;
        }
        }
    }
    else
    {
        qDebug() << ""
    }
}

Account AccountService::getAccount() const
{
    return mainAccount;
}

bool AccountService::updateAccount(const Account &info)
{
    if (info.id != account.id)
    {
        qWarning() << "The ID of the passed account does not match with the stored account.";
        return false;
    }
    bool successful = false;
    switch (info.role)
    {
    case Account::Artist:
    {
        if (available)
        {
            successful = ArtistRepository::instance().save(info);
        }
        break;
    }
    case Account::Listener:
    {
        if (available)
        {
            successful = ListenerRepository::instance().save(info);
        }
        break;
    }
    }
    return false;
}

AccountService *AccountService::createAccount(const Account &data)
{

}

