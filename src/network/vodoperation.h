#ifndef VODOPERATION_H
#define VODOPERATION_H

#include <QObject>
#include <QNetworkAccessManager>
#include "urls.h"

//Fetches the network streams data for player

class VodOperation: public QNetworkAccessManager
{
    Q_OBJECT

    void getExtractionData();
    void getM3U(QString url);

public:
    VodOperation();
    ~VodOperation();

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

#endif // VODOPERATION_H
