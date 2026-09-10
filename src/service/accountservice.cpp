#include <optional>
#include "accountservice.h"
#include "artistrepository.h"
#include "listenerrepository.h"

AccountService::AccountService(QObject *parent) : AbstractService {parent} {}

Account AccountService::account() const
{
    return mainAccount;
}

bool AccountService::updateAccount(const AccountData &info)
{
    auto target = Account {};
    auto result = std::optional<ID> { std::nullopt };
    target.id = mainAccount.id;
    target.data = info;
    switch (info.role)
    {
    case AccountData::Artist:
    {
        result = ArtistRepository::instance().save(target);
        break;
    }
    case AccountData::Listener:
    {
        result = ListenerRepository::instance().save(target);
        break;
    }
    case AccountData::Unknown:
        break;
    }
    if (result.has_value())
    {
        mainAccount = target;
        emit accountUpdated(info);
    }
    return result.has_value();
}

bool AccountService::isValid() const
{
    return mainAccount.id != INVALID_ID;
}

std::optional<ID> AccountService::createAccount(const AccountData &data)
{
    switch (data.role)
    {
    case AccountData::Artist:
    {
        return ArtistRepository::instance().save(Account(data));
    }
    case AccountData::Listener:
    {
        return ListenerRepository::instance().save(Account(data));
    }
    default:
    {
        return std::nullopt;
    }
    }
}
