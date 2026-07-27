#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QStringList>

class DatabaseManager
{
public:
    static DatabaseManager& instance();
    void initialize();
};

#endif // DATABASEMANAGER_H
