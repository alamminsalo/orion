#include "vodsearchoperation.h"
#include "../util/jsonparser.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

VodSearchOperation::VodSearchOperation()
{
    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleReply(QNetworkReply*)));
}

VodSearchOperation::~VodSearchOperation()
{

}

void VodSearchOperation::search(const QString channelName, quint32 offset, quint32 limit)
{
    QString url = QString(KRAKEN_API)
            + QString("/channels/%1/videos").arg(channelName)
            + QString("?offset=%1").arg(offset)
            + QString("&limit=%1").arg(limit)
            + "&hls=true";

    QNetworkRequest request;
    request.setUrl(QUrl(url));

    get(request);
}

QList<Vod *> &VodSearchOperation::getResult()
{
    return result;
}

void VodSearchOperation::handleReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }

    QByteArray data = reply->readAll();

    result = JsonParser::parseVods(data);

    emit operationFinished();
}
