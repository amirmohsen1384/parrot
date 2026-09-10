#ifndef ABSTRACTSERVICE_H
#define ABSTRACTSERVICE_H

#include <QObject>

class AbstractService : public QObject
{
    Q_OBJECT
public:
    explicit AbstractService(QObject *parent = nullptr) : QObject{parent} {}
    virtual bool isValid() const = 0;
};

#include "domain.h"

#endif // ABSTRACTSERVICE_H
