#ifndef ARTISTSERVICE_H
#define ARTISTSERVICE_H

#include "accountservice.h"

class ArtistService : public AccountService
{
    Q_OBJECT
public:
    ArtistService(const ID &artistId, QObject *parent = nullptr);
    SongList singles() const;
    AlbumList albums() const;
    SongList songs() const;

public slots:
    virtual bool updateAccount(const Account &info) override;
};

#endif // ARTISTSERVICE_H
