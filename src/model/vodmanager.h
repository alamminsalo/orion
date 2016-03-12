#ifndef VODMANAGER_H
#define VODMANAGER_H

#include <QObject>
#include "vodlistmodel.h"
#include "../network/vodsearchoperation.h"
#include "../network/vodstreamextractoperation.h"

class VodManager: public QObject
{
    Q_OBJECT

public:
    VodManager();
    ~VodManager();

    Q_INVOKABLE void search(const QString channelName, const quint32 offset, const quint32 limit);

    Q_INVOKABLE QString getGame() const;

    Q_INVOKABLE void getStreams(const QString vodId);

    VodStreamExtractOperation *getVodGet() const;

    Q_INVOKABLE QStringList getResults() const;

    VodListModel *getModel() const;

public slots:
    void onSearchFinished();
    void onStreamGetFinished();

signals:
    void searchStarted();
    void searchFinished();
    void streamsGetFinished();

private:
    QString game;
    VodListModel *model;
    VodSearchOperation *vodSearch;
    VodStreamExtractOperation *vodGet;
};

#endif // VODMANAGER_H
