/*
 * Copyright © 2015-2016 Antti Lamminsalo
 *
 * This file is part of Orion.
 *
 * Orion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with Orion.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef M3U8PARSER_H
#define M3U8PARSER_H

#include <QVariantMap>
#include <QMap>
#include <QByteArray>

namespace m3u8 {

    static QVariantMap getUrls(const QByteArray &data)
    {
        QVariantMap streams;

        QString streamName;
        foreach(QString str, QString(data).split("\n")){

            if (str.contains("VIDEO=")){
                str.remove(0, str.indexOf("VIDEO=") + 6);
                str.replace("\"","");
                streamName = str;
            }
            else if (!streamName.isEmpty() && str.startsWith("http://")){

                if (streamName == "chunked")
                    streamName = "source";

                streams.insert(streamName, str);

//                qDebug() << lastKey << ", " << str;

//                if (lastKey == "mobile")
//                    streams[0] = str;

//                else if (lastKey == "low")
//                    streams[1] = str;

//                else if (lastKey == "medium")
//                    streams[2] = str;

//                else if (lastKey == "high")
//                    streams[3] = str;

//                else if (lastKey == "chunked")
//                    streams[4] = str;

                streamName.clear();
            }
        }

        return streams;
    }
}

#endif // M3U8PARSER_H
