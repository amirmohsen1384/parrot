#include "albumservice.h"
#include "songrepository.h"
#include "albumrepository.h"
#include "artistrepository.h"

AlbumService::AlbumService(ID albumId, QObject *parent) : AbstractService(parent)
{
    auto result = AlbumRepository::instance().search(albumId);
    if (!result.has_value())
    {
        qWarning() << "No album associated with ID" << albumId << "found.";
    }
    else
    {
        mainAlbum = *result;
    }
}

Album AlbumService::album() const
{
    return mainAlbum;
}

Account AlbumService::artist() const
{
    auto result = ArtistRepository::instance().search(mainAlbum.ownerId);
    if (result.has_value())
    {
        return *result;
    }
    else
    {
        return {};
    }
}

SongList AlbumService::songs() const
{
    return SongRepository::instance().byAlbum(mainAlbum.id);
}

bool AlbumService::isValid() const
{
    return mainAlbum.id != INVALID_ID;
}

bool AlbumService::removeSong(ID songId)
{
    auto result = SongRepository::instance().search(songId);
    if (!result)
    {
        qWarning() << "Failed to remove the song with ID" << songId << ":" << "Failed to find the song.";
        return false;
    }
    else if (result->albumId != mainAlbum.id || result->ownerId != mainAlbum.ownerId)
    {
        qWarning() << "Failed to remove the song with ID" << songId << ":" << "The song does not belong to the artist or album.";
        return false;
    }
    else
    {
        auto success = SongRepository::instance().remove(songId);
        if (success)
        {
            emit songRemoved(songId);
        }
        return success;
    }
}

bool AlbumService::addSong(const SongData &data)
{
    auto target = Song {};
    target.ownerId = mainAlbum.ownerId;
    target.albumId = mainAlbum.id;
    target.data = data;
    auto result = SongRepository::instance().save(target);
    if (result.has_value())
    {
        emit songAdded(result.value());
    }
    return result.has_value();
}

bool AlbumService::updateAlbum(const AlbumData &data)
{
    auto result = std::optional<ID> { std::nullopt };
    auto target = Album {};
    target.ownerId = mainAlbum.ownerId;
    target.id = mainAlbum.id;
    target.data = data;
    result = AlbumRepository::instance().save(target);
    if (result.has_value())
    {
        mainAlbum = target;
        emit albumUpdated(mainAlbum);
    }
    return result.has_value();
}
