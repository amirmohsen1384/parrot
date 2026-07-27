#ifndef ABSTRACTREPOSITORY_H
#define ABSTRACTREPOSITORY_H

#include "domain.h"

class QSqlQuery;

template<typename T>
class AbstractRepository
{
public:
    virtual std::optional<T> search(ID value) const = 0;
    virtual std::optional<ID> save(const T &value) = 0;
    virtual bool remove(ID value) = 0;
};

#endif // ABSTRACTREPOSITORY_H
