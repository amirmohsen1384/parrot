#ifndef AUTHENTICATIONSERVICE_H
#define AUTHENTICATIONSERVICE_H

#include "accountservice.h"
#include <expected>

class AuthenticationService : public AbstractService
{
    Q_OBJECT
public:
    enum AuthenticationError {UserNotFound, IncorrectPassword, UnknownAuthentication};

    explicit AuthenticationService(QObject *parent = nullptr);
    static std::expected<std::unique_ptr<AccountService>, AuthenticationError> login(
        const QString &userName,
        const QString &password,
        const AccountData::Role &role
    );
};

#endif // AUTHENTICATIONSERVICE_H
