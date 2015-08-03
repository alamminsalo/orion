#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include "channel.h"
#include "../util/fileutils.h"
//#include "../util/connector.h"
//#include "../util/threadman.h"
#include "../network/networkmanager.h"
//#include "../rapidjson/include/rapidjson/document.h"
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <QStringRef>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#define TWITCH_URI "https://api.twitch.tv/kraken"
#define DATAURI "./data.json"
#define DEFAULT_LOGO_URL "http://static-cdn.jtvnw.net/jtv_user_pictures/xarth/404_user_150x150.png"

//class ThreadManager;
class NetworkManager;

class ChannelManager: public QObject{
    Q_OBJECT
	protected:
		unsigned int update_counter, check_counter;
        std::vector<Channel*> channels;
        //ThreadManager* tman;
        NetworkManager* netman;

	
	public:
		ChannelManager();

		~ChannelManager();	
        void load();
        void save();
        bool readJSON(const QString&);
        bool writeJSON(const QString&);
		void checkStream(Channel*,bool);

        void updateChannels();
        void add(Channel *channel);
		void add(const char*,const char*,const char*,const char*);
		void remove(const char*);
        void remove(Channel*);
		void add(const char*);
		void printList();
        void setAlert(const char*,const char*);
        Channel *find(const QString&);
        int findPos(const QString&);
        std::vector<Channel*> *getChannels(){ return &channels; }
		void clearData();
        void play(Channel*);
        Channel* getLastAdded();
        bool channelExists(const char*);
        void checkStreams();
        void parseStreams(const QJsonObject&);

        bool parseChannelDataToJSON(std::string);
        int parseStreamDataToJSON(std::string);

        void parseStream(const QJsonObject, Channel* channel = 0);
        void parseChannel(const QJsonObject, Channel*);

        void checkResources();
        void getPreviewForChannel(Channel*);

    signals:
        void channelExists(Channel*);
        void channelNotFound(Channel*);
        void channelStateChanged(Channel*);
        void newChannel(Channel*);
};

#endif //CHANNEL_MANAGER_H
