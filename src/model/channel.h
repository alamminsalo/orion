#ifndef CHANNEL_H
#define CHANNEL_H

#include "../util/fileutils.h"
#include <QObject>
#include <QJsonObject>
#include <QVariantMap>
#include <QDateTime>
#include <QDebug>

class Channel: public QObject{

    Q_OBJECT
    Q_PROPERTY(QString name READ getName NOTIFY updated)
    Q_PROPERTY(QString logo READ getLogourl NOTIFY updated)
    Q_PROPERTY(QString info READ getInfo NOTIFY updated)
    Q_PROPERTY(QString preview READ getPreviewurl NOTIFY updated)
    Q_PROPERTY(bool online READ isOnline NOTIFY updated)
    Q_PROPERTY(quint32 viewers READ getViewers NOTIFY updated())

	private:
    QString name;
    QString uri;
    QString info;
    bool alert;
    QString game;
    quint32 viewers;
	bool online;
    bool changed;
    qint64 timestamp;
    QString logopath;
    QString previewpath;
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
        void setURIName(const QString&);
        void setInfo(const QString&);
        void setAlert(bool);
        void setLastSeen(time_t);
        const QString lastOnline();
		void updateTime();
        qint64 getTime();
        const QString getName();
        const QString getUriName();
        const QString getFullUri();
        const QString getInfo();
		bool hasAlert();
		void setOnline(const bool);
        bool isOnline();
        void setLogoPath(const QString&);
        const QString getLogoPath();
        void setPreviewPath(const QString&);
        const QString getPreviewPath();
        bool isChanged();
        void setChanged(bool b);

        const QString getLogourl();
        void setLogourl(const QString&);

        const QString getPreviewurl();
        void setPreviewurl(const QString&);

        quint32 getViewers() const;
        void setViewers(quint32 value);

        const QString getGame() const;
        void setGame(const QString&);

        static bool greaterThan(Channel*, Channel*);

signals:
        void updated();
        void iconUpdated();
        void status404();

};
Q_DECLARE_METATYPE(Channel)

#endif //CHANNEL_H
