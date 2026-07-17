#ifndef ALBUMREPOSITORY_H
#define ALBUMREPOSITORY_H

#include "data/abstractrepository.h"

class AlbumRepository : public AbstractRepository<Album>
{
private:
    AlbumRepository() = default;
    Q_DISABLE_COPY_MOVE(AlbumRepository)

public:
    std::optional<Album> search(ID value) const override;
    std::optional<ID> save(const Album &value) override;
    static AlbumRepository& instance();
    bool remove(ID value) override;

    AlbumList albums(ID artistId) const;

private:
    static Album fromQuery(const QSqlQuery &query);
    std::optional<ID> insert(const Album &value);
    bool update(const Album &value);
};

#endif // ALBUMREPOSITORY_H
