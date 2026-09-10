#include "genremodel.h"
#include <QFileInfo>
#include <QDir>
#include <QHash>

namespace
{
    SongData::Genre genreFromFileName(const QString &baseName)
    {
        static const QHash<QString, SongData::Genre> map
        {
            {QStringLiteral("pop"), SongData::Genre::Pop},
            {QStringLiteral("rap"), SongData::Genre::Rap},
            {QStringLiteral("jazz"), SongData::Genre::Jazz},
            {QStringLiteral("rock"), SongData::Genre::Rock},
            {QStringLiteral("folk"), SongData::Genre::Folk},
            {QStringLiteral("classic"), SongData::Genre::Classic},
            {QStringLiteral("country"), SongData::Genre::Country},
        };
        return map.value(baseName.toLower(), SongData::Genre::Unknown);
    }

    QString displayNameFor(const QString &baseName)
    {
        if (baseName.isEmpty())
            return {};
        return baseName.left(1).toUpper() + baseName.mid(1).toLower();
    }
}

GenreModel::GenreModel(QObject *parent)
    : QAbstractListModel(parent)
{
    const QDir dir(QStringLiteral(":/genres"));
    const QStringList files = dir.entryList({QStringLiteral("*.png")}, QDir::Files, QDir::Name);

    for (const QString &fileName : files) {
        const QString baseName = QFileInfo(fileName).baseName();
        const SongData::Genre genre = genreFromFileName(baseName);
        if (genre == SongData::Genre::Unknown)
            continue;

        Entry entry;
        entry.genre = genre;
        entry.name = displayNameFor(baseName);
        entry.icon = QIcon(dir.filePath(fileName));
        m_genres.append(entry);
    }
}

int GenreModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_genres.size());
}

QVariant GenreModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_genres.size())
        return {};

    const Entry &entry = m_genres.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
        return entry.name;
    case Qt::DecorationRole:
        return entry.icon;
    case GenreRole:
        return QVariant::fromValue(entry.genre);
    default:
        return {};
    }
}
