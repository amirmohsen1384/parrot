#include <QApplication>
#include "data/databasemanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    DatabaseManager::instance().initialize();

    return app.exec();
}
