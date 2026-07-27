#ifndef ARTISTSERVICE_H
#define ARTISTSERVICE_H

#include "accountservice.h"

class ArtistService : public AccountService
{
    Q_OBJECT
public:
    ArtistService(ID artistId, QObject *parent = nullptr);

public:
    SongList singles() const;
    AlbumList albums() const;
    SongList songs() const;

public slots:
    void addAlbum(const AlbumData &data);
    bool removeAlbum(ID albumId);

signals:
    void albumAdded(const Album &album);
    void albumRemoved(ID value);
};

#endif // ARTISTSERVICE_H
