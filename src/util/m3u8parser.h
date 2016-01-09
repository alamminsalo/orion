#ifndef M3U8PARSER_H
#define M3U8PARSER_H

#include <QVariantMap>
#include <QMap>
#include <QByteArray>

namespace m3u8 {

    static QMap<quint32, QString> qualityMap;

    static QString getUrl(const QByteArray &data,qint32 quality)
    {
        if (qualityMap.isEmpty()){
            //Initialize reference map
            qualityMap[0] = "mobile";
            qualityMap[1] = "low";
            qualityMap[2] = "medium";
            qualityMap[3] = "high";
            qualityMap[4] = "chunked";
        }

        QVariantMap streamQualityMap;

        QString lastKey;
        foreach(QString str, QString(data).split("\n")){

            if (str.contains("VIDEO=")){
                str.remove(0, str.indexOf(QRegExp("\".+\""))).replace("\"","");
                lastKey = str;
            }
            else if (!lastKey.isEmpty() && str.startsWith("http://")){
                streamQualityMap[lastKey] = str;
                lastKey.clear();
            }
        }


        qDebug() << "Requested quality: " << qualityMap[quality];

        qDebug() << "Available quality levels for stream: ";
        foreach (QString key, streamQualityMap.keys()){
            qDebug() << key;
        }

        while (quality >= 0 && (!qualityMap.contains(quality) || !streamQualityMap.contains(qualityMap[quality]))){
            quality--;
        }

        if (quality < 0){
            qDebug() << "Couldn't resolve quality, returning last possible entry";
        } else {
            qDebug() << "Resolved quality: " << qualityMap[quality];
        }

        return streamQualityMap.contains(qualityMap[quality]) ?
                    streamQualityMap[qualityMap[quality]].toString() : streamQualityMap.values().last().toString();
    }
}

#endif // M3U8PARSER_H
