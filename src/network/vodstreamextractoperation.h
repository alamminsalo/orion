#ifndef VODSTREAMEXTRACTOPERATION_H
#define VODSTREAMEXTRACTOPERATION_H

#include <QObject>
#include <QNetworkAccessManager>
#include "urls.h"

//Fetches the network streams data for player

class VodStreamExtractOperation: public QNetworkAccessManager
{
    Q_OBJECT

    void getExtractionData();
    void getM3U(QString url);

public:
    VodStreamExtractOperation();
    ~VodStreamExtractOperation();

    Q_INVOKABLE void run(const QString vod_id);

    QString getVod() const;
    void setVod(const QString &value);

    Q_INVOKABLE QStringList getResult() const;

signals:
    void operationFinished();

public slots:
    void handleReply(QNetworkReply* reply);

private:
    QString vod;
    QStringList result;
    quint8 phase;
};

#endif // VODSTREAMEXTRACTOPERATION_H
