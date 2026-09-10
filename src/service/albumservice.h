#ifndef ALBUMSERVICE_H
#define ALBUMSERVICE_H

#include "abstractservice.h"

class AlbumService : public AbstractService
{
    Q_OBJECT
public:
    AlbumService(ID albumId, QObject *parent = nullptr);

    Album album() const;
    Account artist() const;
    SongList songs() const;
    virtual bool isValid() const override;

public slots:
    bool removeSong(ID songId);
    bool addSong(const SongData &data);
    bool updateAlbum(const AlbumData &data);

signals:
    void songAdded(ID songId);
    void songRemoved(ID songId);
    void albumUpdated(const Album &album);

private:
    Album mainAlbum;
};

#endif // ALBUMSERVICE_H
