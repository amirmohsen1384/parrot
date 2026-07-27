#include "authenticationservice.h"
#include "listenerrepository.h"
#include "artistrepository.h"
#include "listenerservice.h"
#include "artistservice.h"

AuthenticationService::AuthenticationService(QObject *parent) : AbstractService{parent}
{}

std::expected<std::unique_ptr<AccountService>, AuthenticationService::AuthenticationError> AuthenticationService::login(
    const QString &userName,
    const QString &password,
    const AccountData::Role &role
)
{
    auto result = std::optional<Account> {};
    switch(role)
    {
    case AccountData::Artist:
    {
        result = ArtistRepository::instance().searchByUserName(userName);
        break;
    }
    case AccountData::Listener:
    {
        result = ListenerRepository::instance().searchByUserName(userName);
        break;
    }
    }
    if (!result.has_value())
    {
        return std::unexpected(UserNotFound);
    }
    else
    {
        if (result->data.password != password)
        {
            return std::unexpected(IncorrectPassword);
        }
        else
        {
            switch(role)
            {
            case AccountData::Artist:
            {
                return std::move(std::make_unique<ArtistService>(result->id));
            }
            case AccountData::Listener:
            {
                return std::move(std::make_unique<ListenerService>(result->id));
            }
            }
        }
    }
    return std::unexpected(UnknownAuthentication);
}
