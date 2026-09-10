#include "songrepository.h"
#include "playlistservice.h"
#include "listenerrepository.h"
#include "playlistrepository.h"

PlaylistService::PlaylistService(ID playlistId, QObject *parent) : AbstractService{parent}
{
    auto result = PlaylistRepository::instance().search(playlistId);
    if (!result.has_value())
    {
        qWarning() << "No playlist associated with ID" << playlistId << "found.";
    }
    else
    {
        mainPlaylist = *result;
    }
}

Account PlaylistService::owner() const
{
    auto result = ListenerRepository::instance().search(mainPlaylist.ownerId);
    if (result.has_value())
    {
        return *result;
    }
    else
    {
        return {};
    }
}

SongList PlaylistService::songs() const
{
    return SongRepository::instance().byPlaylist(mainPlaylist.id);
}

Playlist PlaylistService::playlist() const
{
    return mainPlaylist;
}

bool PlaylistService::isValid() const
{
    return mainPlaylist.id != INVALID_ID;
}

bool PlaylistService::removeSong(ID songId)
{
    auto result = SongRepository::instance().search(songId);
    if (!result)
    {
        qWarning() << "Failed to remove the song with ID" << songId << ":" << "Failed to find the song.";
        return false;
    }
    else
    {
        auto success = PlaylistRepository::instance().removeSong(mainPlaylist.id, songId);
        if (success)
        {
            emit songRemoved(songId);
        }
        return success;
    }
}

bool PlaylistService::addSong(ID songId)
{
    auto result = SongRepository::instance().search(songId);
    if (!result)
    {
        qWarning() << "Failed to remove the song with ID" << songId << ":" << "Failed to find the song.";
        return false;
    }
    else
    {
        auto success = PlaylistRepository::instance().insertSong(mainPlaylist.id, songId);
        if (success.has_value())
        {
            emit songAdded(songId);
        }
        return success.has_value();
    }
}

bool PlaylistService::updatePlaylist(const PlaylistData &data)
{
    auto result = std::optional<ID> { std::nullopt };
    auto target = Playlist {};
    target.ownerId = mainPlaylist.ownerId;
    target.id = mainPlaylist.id;
    target.data = data;
    result = PlaylistRepository::instance().save(target);
    if (result.has_value())
    {
        mainPlaylist = target;
        emit playlistUpdated(mainPlaylist);
    }
    return result.has_value();
}
