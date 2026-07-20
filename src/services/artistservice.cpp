#include <QDebug>
#include "artistservice.h"
#include "data/songrepository.h"
#include "data/albumrepository.h"
#include "data/artistrepository.h"

ArtistService::ArtistService(const ID &artistId, QObject *parent) : AccountService{parent}
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

bool ArtistService::updateAccount(const Account &info)
{
    if (info.id == mainAccount.id)
    {
        auto result = ArtistRepository::instance().save(info);
        auto sample = result.has_value() && *result == mainAccount.id;
        if (sample)
        {
            emit accountUpdated(mainAccount);
        }
        return sample;
    }
    else
    {
        qWarning() << "Failed to update account data:" << "The account ID does not match with the target account.";
        return false;
    }
}
