#ifndef M3U8PARSER_H
#define M3U8PARSER_H

#include <QVariantMap>
#include <QMap>
#include <QByteArray>

namespace m3u8 {

    static QStringList getUrls(const QByteArray &data)
    {


        QStringList streams;

        streams.push_back("");
        streams.push_back("");
        streams.push_back("");
        streams.push_back("");
        streams.push_back("");

        QString lastKey;
        foreach(QString str, QString(data).split("\n")){

            if (str.contains("VIDEO=")){
                str.remove(0, str.indexOf(QRegExp("\".+\""))).replace("\"","");
                lastKey = str;
            }
            else if (!lastKey.isEmpty() && str.startsWith("http://")){

                if (lastKey == "mobile")
                    streams[0] = str;

                else if (lastKey == "low")
                    streams[1] = str;

                else if (lastKey == "medium")
                    streams[2] = str;

                else if (lastKey == "high")
                    streams[3] = str;

                else if (lastKey == "chunked")
                    streams[4] = str;

                lastKey.clear();
            }
        }

        return streams;
    }
}

#endif // M3U8PARSER_H
