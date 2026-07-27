#ifndef ABSTRACTSERVICE_H
#define ABSTRACTSERVICE_H

#include <QObject>
#include "domain.h"

class AbstractService : public QObject
{
    Q_OBJECT
public:
    explicit AbstractService(QObject *parent = nullptr);
    virtual bool isValid() const = 0;
};

#endif // ABSTRACTSERVICE_H
