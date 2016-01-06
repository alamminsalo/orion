#ifndef M3U8PARSER_H
#define M3U8PARSER_H

#include <QVariantMap>
#include <QMap>
#include <QByteArray>

namespace m3u8 {

    static QMap<quint32, QString> qualityMap;

    static QString getUrl(const QByteArray &data,quint32 quality)
    {
        if (qualityMap.isEmpty()){
            //Initialize reference map
            qualityMap[0] = "mobile";
            qualityMap[1] = "low";
            qualityMap[2] = "medium";
            qualityMap[3] = "high";
            qualityMap[4] = "chunked";
        }

        QVariantMap map;

        QString lastKey;
        foreach(QString str, QString(data).split("\n")){

            if (str.contains("VIDEO=")){
                str.remove(0, str.indexOf(QRegExp("\".+\""))).replace("\"","");
                lastKey = str;
            }
            else if (!lastKey.isEmpty() && str.startsWith("http://")){
                map[lastKey] = str;
                lastKey.clear();
            }
        }

        while ((!qualityMap.contains(quality) || !map.contains(qualityMap[quality])) && quality > 0){
            quality--;
        }

        return map.contains(qualityMap[quality]) ? map[qualityMap[quality]].toString() : "";
    }
}

#endif // M3U8PARSER_H
