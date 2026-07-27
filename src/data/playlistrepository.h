#ifndef PLAYLISTREPOSITORY_H
#define PLAYLISTREPOSITORY_H

#include "abstractrepository.h"

class PlaylistRepository : public AbstractRepository<Playlist>
{
private:
    PlaylistRepository() = default;
    Q_DISABLE_COPY_MOVE(PlaylistRepository)

public:
    virtual bool remove(ID playlistId) override;
    virtual std::optional<ID> save(const Playlist &value) override;
    virtual std::optional<Playlist> search(ID playlistId) const override;

    std::optional<ID> insertSong(ID playlistId, ID songId);
    bool removeSong(ID playlistId, ID songId);
    PlaylistList playlists(ID listenerId) const;

public:
    static PlaylistRepository& instance();

private:
    std::optional<ID> insert(const PlaylistData &playlist);
    static Playlist fromQuery(const QSqlQuery &query);
    std::optional<ID> update(const Playlist &playlist);
};

#endif // PLAYLISTREPOSITORY_H
