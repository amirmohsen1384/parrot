#ifndef LISTENERSERVICE_H
#define LISTENERSERVICE_H

#include "accountservice.h"

class ListenerService : public AccountService
{
    Q_OBJECT
public:
    ListenerService(ID listenerId, QObject *parent = nullptr);

    bool isLiked(ID songId);
    bool setLiked(ID songId, bool liked = true);

    SongList likedSongs() const;
    PlaylistList playlists() const;

public slots:

signals:
    void likeChanged(ID songId);
};

#endif // LISTENERSERVICE_H
