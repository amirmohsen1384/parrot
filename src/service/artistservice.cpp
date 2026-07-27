#include <QDebug>
#include "artistservice.h"
#include "songrepository.h"
#include "albumrepository.h"
#include "artistrepository.h"

ArtistService::ArtistService(ID artistId, QObject *parent) : AccountService{parent}
{
    auto result = ArtistRepository::instance().search(artistId);
    if (!result.has_value())
    {
        qWarning() << "No artist associated with ID" << artistId << "found.";
    }
    else
    {
        mainAccount = *result;
    }
}

SongList ArtistService::singles() const
{
    return SongRepository::instance().singleSongs(mainAccount.id);
}

AlbumList ArtistService::albums() const
{
    return AlbumRepository::instance().albums(mainAccount.id);
}

SongList ArtistService::songs() const
{
    return SongRepository::instance().byArtist(mainAccount.id);
}

void ArtistService::addAlbum(const AlbumData &album)
{
    Album target;
    target.data = album;
    target.ownerId = mainAccount.id;
    auto result = AlbumRepository::instance().save(target);
    if (!result.has_value())
    {
        qWarning() << "Failed to add the album for the artist.";
    }
    else
    {
        target.id = *result;
        emit albumAdded(target);
    }
}

bool ArtistService::removeAlbum(ID albumId)
{
    if (albumId == INVALID_ID)
    {
        qWarning() << "Failed to remove the album:" << "You cannot remove the singles album.";
        return false;
    }
    auto result = AlbumRepository::instance().search(albumId);
    if (!result)
    {
        qWarning() << "Failed to remove the album with ID" << albumId << ":" << "Failed to find the album.";
        return false;
    }
    else if (result->ownerId != mainAccount.id)
    {
        qWarning() << "Failed to remove the album with ID" << albumId << ":" << "The album does not belong to the artist.";
        return false;
    }
    else
    {
        auto success = AlbumRepository::instance().remove(albumId);
        if (success)
        {
            emit albumRemoved(albumId);
        }
        return success;
    }
}
