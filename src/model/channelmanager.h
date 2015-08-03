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

#define TWITCH_URI "https://api.twitch.tv/kraken"
#define DATAURI "./data.json"
#define DEFAULT_LOGO_URL "http://static-cdn.jtvnw.net/jtv_user_pictures/xarth/404_user_150x150.png"

class NetworkManager;

class ChannelManager: public QObject{
    Q_OBJECT
	protected:
		unsigned int update_counter, check_counter;
        std::vector<Channel*> channels;
        NetworkManager* netman;

	
	public:
		ChannelManager();

		~ChannelManager();	
        void load();
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
        std::vector<Channel*> *getChannels(){ return &channels; }
		void clearData();
        void play(Channel*);
        void checkStreams();
        void parseStreams(const QJsonObject&);

        void parseStream(const QJsonObject, Channel* channel = 0);
        void parseChannel(const QJsonObject, Channel*);

        void checkResources();

    signals:
        void channelExists(Channel*);
        void channelNotFound(Channel*);
        void channelStateChanged(Channel*);
        void newChannel(Channel*);
};

#endif //CHANNEL_MANAGER_H
