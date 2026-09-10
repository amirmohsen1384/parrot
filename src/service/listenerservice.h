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
    void addPlaylist(const PlaylistData &data);
    bool removePlaylist(ID playlistId);

signals:
    void likeChanged(ID songId);
    void playlistAdded(const Playlist &playlist);
    void playlistRemoved(ID value);
};

#endif // LISTENERSERVICE_H
