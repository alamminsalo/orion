#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include "channel.h"
#include "../util/fileutils.h"
#include "../network/networkmanager.h"
#include <vector>
#include <QStringRef>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QProcess>
#include <QJsonObject>
#include <qqmllist.h>

#define TWITCH_URI "https://api.twitch.tv/kraken"
#define DATAURI "./data.json"
#define DEFAULT_LOGO_URL "http://static-cdn.jtvnw.net/jtv_user_pictures/xarth/404_user_150x150.png"

class NetworkManager;

class ChannelManager: public QObject{
    Q_OBJECT
    Q_PROPERTY(QVariant channels READ channelsList NOTIFY channelStateChanged)

    protected:
		unsigned int update_counter, check_counter;
        QList<Channel*> channels;
        NetworkManager* netman;

	
	public:
		ChannelManager();

		~ChannelManager();	
        void load();
        void load(const QString&);
        void save();
        bool readJSON(const QString&);
        bool writeJSON(const QString&);

        void updateChannels();
        void add(Channel *channel);
        void add(const char*,const char*,const char*,const char*);
		void remove(const char*);
        void remove(Channel*);
        void add(const QString&);
        Channel *find(const QString&);
        int findPos(const QString&);
        QList<Channel*>& getChannels(){ return channels; }
		void clearData();
        void play(Channel*);
        void parseStreams(const QJsonObject&);

        void parseStream(const QJsonObject, Channel* channel = 0);
        void parseChannel(const QJsonObject, Channel*);
        void checkResources();


    signals:
        void channelExists(Channel*);
        void channelNotFound(Channel*);
        void channelStateChanged(Channel*);
        void newChannel(Channel*);

    public slots:
        void checkStreams();
        void notify(Channel*);
        QVariantList channelsList(){
            qDebug() << "get channels called";
            QVariantList list;

            qSort(this->channels.begin(), this->channels.end(), Channel::greaterThan);

            foreach (Channel* channel, channels){
                list.append(QVariant::fromValue(channel));
            }

            return list;
        }

};

#endif //CHANNEL_MANAGER_H
