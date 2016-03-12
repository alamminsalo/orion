#include "vodoperation.h"
#include "../util/jsonparser.h"
#include "../util/m3u8parser.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

VodOperation::VodOperation()
{
    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleReply(QNetworkReply*)));
}

VodOperation::~VodOperation()
{
    result.clear();
}

QString VodOperation::getVod() const
{
    return vod;
}

void VodOperation::setVod(const QString &value)
{
    vod = value;
}

QStringList VodOperation::getResult() const
{
    return result;
}

void VodOperation::run(const QString vod_id)
{
    vod = vod_id;

    //Remove first letter
    vod.remove(QRegExp("[^0-9]"));

    getExtractionData();
}

void VodOperation::getExtractionData()
{
    phase = 0;

    QString url = QString(TWITCH_API)
            + QString("/vods/%1").arg(vod)
            + QString("/access_token");
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    get(request);
}

void VodOperation::getM3U(QString url)
{
    phase = 1;

    QNetworkRequest request;
    request.setUrl(QUrl(url));

    qDebug() << request.url().toEncoded();

    qDebug() << "VOD: " << url;

    get(request);
}

void VodOperation::handleReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }

    QByteArray data = reply->readAll();

    //Access token
    if (phase == 0)
    {
        getM3U(JsonParser::parseVodExtractionInfo(data, vod));
        phase = 1;
    }

    //Extract data
    else
    {
        result = m3u8::getUrls(data);
        emit operationFinished();
    }
}

