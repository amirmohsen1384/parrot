#include "databasemanager.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager database;
    return database;
}

void DatabaseManager::initialize()
{
    using TableInfo = std::pair<QString, QString>;
    static const QList<TableInfo> tables = {
        {
            "Listeners",
            R"(CREATE TABLE IF NOT EXISTS Listeners(
            id INTEGER PRIMARY KEY,
            user_name TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL,
            name TEXT NOT NULL,
            biography TEXT NOT NULL,
            photo BLOB
        ))"
        },
        {
            "Artists",
            R"(CREATE TABLE IF NOT EXISTS Artists(
            id INTEGER PRIMARY KEY,
            user_name TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL,
            name TEXT NOT NULL,
            biography TEXT NOT NULL,
            photo BLOB
        ))"
        },
        {
            "Albums",
            R"(CREATE TABLE IF NOT EXISTS Albums(
            id INTEGER PRIMARY KEY,
            name TEXT NOT NULL,
            artist_id INTEGER NOT NULL,
            cover BLOB,
            FOREIGN KEY (artist_id) REFERENCES Artists(id) ON DELETE CASCADE
        ))"
        },
        {
            "Songs",
            R"(CREATE TABLE IF NOT EXISTS Songs(
            id INTEGER PRIMARY KEY,
            file_name TEXT UNIQUE NOT NULL,
            released_year INTEGER NOT NULL,
            genre INTEGER NOT NULL,
            name TEXT NOT NULL,
            artist_id INTEGER NOT NULL,
            album_id INTEGER,
            cover BLOB,
            FOREIGN KEY (album_id) REFERENCES Albums(id) ON DELETE SET NULL,
            FOREIGN KEY (artist_id) REFERENCES Artists(id) ON DELETE CASCADE
        ))"
        },
        {
            "Playlists",
            R"(CREATE TABLE IF NOT EXISTS Playlists(
            id INTEGER PRIMARY KEY,
            owner_id INTEGER NOT NULL,
            name TEXT NOT NULL,
            FOREIGN KEY (owner_id) REFERENCES Listeners(id) ON DELETE CASCADE
        ))"
        },
        {
            "PlaylistSongs",
            R"(CREATE TABLE IF NOT EXISTS PlaylistSongs(
            id INTEGER PRIMARY KEY,
            song_id INTEGER NOT NULL,
            playlist_id INTEGER NOT NULL,
            FOREIGN KEY (song_id) REFERENCES Songs(id) ON DELETE CASCADE,
            FOREIGN KEY (playlist_id) REFERENCES Playlists(id) ON DELETE CASCADE
        ))"
        },
        {
            "LikedSongs",
            R"(CREATE TABLE IF NOT EXISTS LikedSongs(
            id INTEGER PRIMARY KEY,
            song_id INTEGER NOT NULL,
            listener_id INTEGER NOT NULL,
            UNIQUE(listener_id, song_id),
            FOREIGN KEY (song_id) REFERENCES Songs(id) ON DELETE CASCADE,
            FOREIGN KEY (listener_id) REFERENCES Listeners(id) ON DELETE CASCADE
        ))"
        }
    };

    QSqlDatabase database;
    if (QSqlDatabase::contains())
    {
        database = QSqlDatabase::database();
    }
    else
    {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName("repository.db");
    }

    if (!database.isOpen())
    {
        if (!database.open())
        {
            qCritical() << "Failed to open database:" << database.lastError().text();
            return;
        }
    }

    QSqlQuery query(database);
    if(!query.exec("PRAGMA foreign_keys = ON;"))
    {
        qCritical() << "Failed to enable foreign keys:" << query.lastError().text();
    }
    if(!query.exec("PRAGMA journal_mode = WAL;"))
    {
        qCritical() << "Failed to enable journal mode:" << query.lastError().text();
    }

    if (!database.transaction())
    {
        qCritical() << "Couldn't start transaction:" << database.lastError().text();
        return;
    }

    for (const auto &[name, sql] : tables)
    {
        if(!query.exec(sql))
        {
            qCritical() << "Failed to create the" << name << "table:" << query.lastError().text();
        }
    }

    if (!database.commit())
    {
        qCritical() << "Commit failed:" << database.lastError().text();
        database.rollback();
    }
}
