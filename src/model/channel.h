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

#ifndef CHANNEL_H
#define CHANNEL_H

#include "../util/fileutils.h"
#include <QObject>
#include <QJsonObject>
#include <QVariantMap>
#include <QDateTime>
#include <QDebug>

#define DEFAULT_LOGO_URL    "http://static-cdn.jtvnw.net/jtv_user_pictures/xarth/404_user_150x150.png"
#define DEFAULT_LOGO_PATH   "resources/logos/default.png"

class Channel: public QObject{

    Q_OBJECT

	private:
    quint32 id;
    QString name;
    QString serviceName;
    QString info;
    bool alert;
    QString game;
    qint32 viewers;
	bool online;
    bool favourite;
    qint64 timestamp;
    QString logouri;
    QString previewuri;

	public:
        Channel();
        Channel(const QString&);
        Channel(const QString&, const QString&, const QString&);
        Channel(const QString&, const QString&, const QString&, bool);
        Channel(const QString&, const QString&, const QString&, bool, time_t);
        Channel(const QString&, const QString&, const QString&, bool, time_t, const QString&);
        Channel(const QString&, const QString&, const QString&, bool, time_t, const QString&, const QString&);
        Channel(const Channel&);
        ~Channel();
        void updateWith(const Channel &other);
        const QJsonObject getJSON() const;
        void setName(const QString&);
        void setServiceName(const QString&);
        void setInfo(const QString&);
        void setAlert(bool);
        void setLastSeen(time_t);
        const QString lastOnline();
		void updateTime();
        qint64 getTime();
        const QString getName() const;
        const QString getServiceName();
        const QString getFullUri();
        const QString getInfo() const;
		bool hasAlert();
		void setOnline(const bool);
        bool isOnline();

        const QString getLogourl() const;
        void setLogourl(const QString&);

        const QString getPreviewurl();
        void setPreviewurl(const QString&);

        qint32 getViewers() const;
        void setViewers(qint32 value);

        const QString getGame() const;
        void setGame(const QString&);

        static bool greaterThan(Channel*, Channel*);

        quint32 getId() const;
        void setId(const quint32 &value);

        bool isFavourite() const;
        void setFavourite(bool value);

signals:
        void updated();
        void iconUpdated();
        void status404();

};
//Q_DECLARE_METATYPE(Channel)

#endif //CHANNEL_H
