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
        const QJsonObject getJSON() const;
        void setName(const QString&);
        void setServiceName(const QString&);
        void setInfo(const QString&);
        void setAlert(bool);
        void setLastSeen(time_t);
        const QString lastOnline();
		void updateTime();
        qint64 getTime();
        const QString getName();
        const QString getServiceName();
        const QString getFullUri();
        const QString getInfo();
		bool hasAlert();
		void setOnline(const bool);
        bool isOnline();

        const QString getLogourl();
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
