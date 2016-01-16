#ifndef GAMELISTMODEL_H
#define GAMELISTMODEL_H

#include <QAbstractListModel>
#include <QHashData>
#include <QList>
#include <QDebug>
#include "game.h"

class GameListModel: public QAbstractListModel
{
    Q_OBJECT

public:
    GameListModel();
    ~GameListModel();

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QHash<int,QByteArray> roleNames() const;

    void addAll(const QList<Game*> &);
    void addGame(Game*);
    void removeGame(Game*);
    Game* find(const QString&);
    void clear();

    enum Roles {
        NameRole =          Qt::UserRole + 1,
        LogoRole =          Qt::UserRole + 2,
        PreviewRole =       Qt::UserRole + 3,
        ViewersRole =       Qt::UserRole + 4
    };

private:
    QList<Game*> games;

public slots:
    int count();
};


#endif // GAMELISTMODEL_H
