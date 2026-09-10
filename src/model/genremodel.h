#ifndef GENREMODEL_H
#define GENREMODEL_H

#include <QAbstractListModel>
#include <QIcon>

#include "domain.h"

class GenreModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        GenreRole = Qt::UserRole
    };

    explicit GenreModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    struct Entry
    {
        SongData::Genre genre = SongData::Genre::Unknown;
        QString name;
        QIcon icon;
    };

    QList<Entry> m_genres;
};

#endif // GENREMODEL_H
