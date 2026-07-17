#include <QApplication>
#include "src/data/databasemanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    DatabaseManager::instance().initialize();

    return app.exec();
}
