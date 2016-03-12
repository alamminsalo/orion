#ifndef VODSEARCHOPERATION_H
#define VODSEARCHOPERATION_H

//Handles search operations

#include <QObject>
#include <QNetworkAccessManager>
#include <QList>
#include "../model/vod.h"
#include "urls.h"

class VodSearchOperation: public QNetworkAccessManager
{
    Q_OBJECT

public:
    VodSearchOperation();
    ~VodSearchOperation();

    void search(const QString channelName, quint32 offset, quint32 limit);

    QList<Vod *> &getResult();

signals:
    void operationFinished();

public slots:
    void handleReply(QNetworkReply* reply);

private:
    QList<Vod *> result;
};

#endif // VODSEARCHOPERATION_H
