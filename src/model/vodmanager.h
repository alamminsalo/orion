#ifndef VODMANAGER_H
#define VODMANAGER_H

#include <QObject>
#include "../network/networkmanager.h"
#include "vodlistmodel.h"

class VodManager: public QObject
{
    Q_OBJECT

public:
    VodManager(NetworkManager *netman);
    ~VodManager();

    Q_INVOKABLE void search(const QString channelName, const quint32 offset, const quint32 limit);

    Q_INVOKABLE QString getGame() const;

    Q_INVOKABLE void getBroadcasts(const QString vod);

    VodListModel *getModel() const;

public slots:
    void onSearchFinished(QList<Vod *>);
    void onStreamGetFinished(QStringList);

signals:
    void searchStarted();
    void searchFinished();
    void streamsGetFinished(QStringList items);

private:
    QString game;
    VodListModel *model;
    NetworkManager *netman;
};

#endif // VODMANAGER_H
