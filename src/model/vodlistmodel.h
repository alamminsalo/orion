#ifndef VODLISTMODEL_H
#define VODLISTMODEL_H

#include <QAbstractListModel>
#include <QHashData>
#include <QList>
#include <QDebug>
#include "vod.h"

class VodListModel: public QAbstractListModel
{
    Q_OBJECT

public:
    VodListModel();
    ~VodListModel();

    enum Roles {
        Title =          Qt::UserRole + 1,
        Preview,
        Id,
        Game,
        Duration,
        Views
    };

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QHash<int,QByteArray> roleNames() const;

    void addAll(QList<Vod *> &items);
    Vod *find(const QString id);
    void clear();

    Q_INVOKABLE int count() const;

private:
    QList<Vod*> vods;
};

#endif // VODLISTMODEL_H
