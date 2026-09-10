#ifndef PLAYLISTSERVICE_H
#define PLAYLISTSERVICE_H

#include <QObject>
#include "abstractservice.h"

class PlaylistService : public AbstractService
{
    Q_OBJECT
public:
    PlaylistService(ID playlistId, QObject *parent = nullptr);

    Account owner() const;
    SongList songs() const;
    Playlist playlist() const;
    virtual bool isValid() const override;

public slots:
    bool addSong(ID songId);
    bool removeSong(ID songId);
    bool updatePlaylist(const PlaylistData &data);

signals:
    void songAdded(ID songId);
    void songRemoved(ID songId);
    void playlistUpdated(const Playlist &data);

private:
    Playlist mainPlaylist;
};

#endif // PLAYLISTSERVICE_H
