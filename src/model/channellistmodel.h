#ifndef CHANNELCOLLECTION_H
#define CHANNELCOLLECTION_H

#include <QAbstractListModel>
#include <QHashData>
#include <QList>
#include "channel.h"

class ChannelListModel: public QAbstractListModel
{
    Q_OBJECT

public:
    ChannelListModel();
    ~ChannelListModel();

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QHash<int,QByteArray> roleNames() const;

    void updateChannel(Channel*);
    void updateChannels(const QList<Channel*>&);
    void updateStreams(const QList<Channel*>&);
    void updateStream(Channel*);
    void checkStreams();

    void addChannel(Channel*);
    void removeChannel(Channel*);
    Channel* find(const QString&);
    void clear();

    enum Roles {
        NameRole =          Qt::UserRole + 1,
        InfoRole =          Qt::UserRole + 2,
        LogoRole =          Qt::UserRole + 3,
        PreviewRole =       Qt::UserRole + 4,
        OnlineRole =        Qt::UserRole + 5,
        ViewersRole =       Qt::UserRole + 6,
        ServiceNameRole =   Qt::UserRole + 7
    };

    QList<Channel *> getChannels() const;

public slots:
    void updateChannelForView(Channel*);
    int count();

signals:
    void channelOnlineStateChanged(Channel*);

private:
    Q_DISABLE_COPY(ChannelListModel)
    QList<Channel*> channels;

};

#endif // CHANNELCOLLECTION_H
