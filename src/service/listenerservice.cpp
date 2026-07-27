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
