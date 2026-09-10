#include "listenerservice.h"
#include "songrepository.h"
#include "listenerrepository.h"
#include "playlistrepository.h"

ListenerService::ListenerService(ID listenerId, QObject *parent) : AccountService{parent}
{
    auto result = ListenerRepository::instance().search(listenerId);
    if (!result.has_value())
    {
        qWarning() << "No listener associated with ID" << listenerId << "found.";
    }
    else
    {
        mainAccount = *result;
    }
}

bool ListenerService::isLiked(ID songId)
{
    if (!SongRepository::instance().search(songId).has_value())
    {
        qWarning() << "The requested song or account is not available.";
        return false;
    }
    return ListenerRepository::instance().isLiked(mainAccount.id, songId);
}

bool ListenerService::setLiked(ID songId, bool liked)
{
    if (!SongRepository::instance().search(songId).has_value() || mainAccount.id == INVALID_ID)
    {
        qWarning() << "The requested song or account is not available.";
        return false;
    }
    auto result = ListenerRepository::instance().setLiked(mainAccount.id, songId, liked);
    if (result)
    {
        emit likeChanged(songId);
    }
    return result;
}

SongList ListenerService::likedSongs() const
{
    return SongRepository::instance().byLikedSongs(mainAccount.id);
}

PlaylistList ListenerService::playlists() const
{
    return PlaylistRepository::instance().playlists(mainAccount.id);
}

void ListenerService::addPlaylist(const PlaylistData &data)
{
    Playlist target;
    target.data = data;
    target.ownerId = mainAccount.id;
    auto result = PlaylistRepository::instance().save(target);
    if (!result.has_value())
    {
        qWarning() << "Failed to add the playlist for the listener.";
    }
    else
    {
        target.id = *result;
        emit playlistAdded(target);
    }
}

bool ListenerService::removePlaylist(ID playlistId)
{
    if (playlistId == INVALID_ID)
    {
        qWarning() << "Failed to remove the playlist:" << "Invalid playlist ID.";
        return false;
    }
    auto result = PlaylistRepository::instance().search(playlistId);
    if (!result)
    {
        qWarning() << "Failed to remove the playlist with ID" << playlistId << ":" << "Failed to find the playlist.";
        return false;
    }
    else if (result->ownerId != mainAccount.id)
    {
        qWarning() << "Failed to remove the playlist with ID" << playlistId << ":" << "The playlist does not belong to the listener.";
        return false;
    }
    else
    {
        auto success = PlaylistRepository::instance().remove(playlistId);
        if (success)
        {
            emit playlistRemoved(playlistId);
        }
        return success;
    }
}
