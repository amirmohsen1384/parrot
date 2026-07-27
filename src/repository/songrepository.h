#ifndef SONGREPOSITORY_H
#define SONGREPOSITORY_H

#include "abstractrepository.h"
#include <QSqlQuery>

class SongRepository : public AbstractRepository<Song>
{
private:
    SongRepository() = default;
    Q_DISABLE_COPY_MOVE(SongRepository)

public:
    virtual std::optional<ID> save(const Song &value) override;
    virtual std::optional<Song> search(ID value) const override;
    virtual bool remove(ID value) override;
    static SongRepository& instance();

public:
    SongList byLikedSongs(ID accountId) const;
    SongList byPlaylist(ID playlistId) const;
    SongList singleSongs(ID artistId) const;
    SongList byArtist(ID artistId) const;
    SongList byAlbum(ID albumId) const;

private:
    std::optional<ID> insert(const Song &value);
    std::optional<ID> update(const Song &value);
    static Song fromQuery(const QSqlQuery &query);
};

#endif // SONGREPOSITORY_H
