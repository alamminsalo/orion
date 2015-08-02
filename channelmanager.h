#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include "channel.h"
#include "fileutils.h"
#include "connector.h"
#include "rapidjson/include/rapidjson/document.h"
#include <vector>
#include <iostream>
#include <stdlib.h>
#include "threadman.h"
#include "kstream/mainwindow.h"
#include <QObject>

#define TWITCH_URI "https://api.twitch.tv/kraken"
#define DATAURI "./data.json"
#define DEFAULT_LOGO_URL "http://static-cdn.jtvnw.net/jtv_user_pictures/xarth/404_user_150x150.png"

class ThreadManager;
class MainWindow;

class ChannelManager: public QObject{
    Q_OBJECT
	protected:
		unsigned int update_counter, check_counter;
        std::vector<Channel*> channels;
        ThreadManager* tman;

        void parseStream(const rapidjson::Value&);
        void parseChannel(const rapidjson::Document&);

	
	public:
		ChannelManager();

		~ChannelManager();	
        void load();
        void save();
		bool readJSON(const char*);
		bool writeJSON(const char*);
		void checkStream(Channel*,bool);

        void checkStreams(bool sync);

        void updateChannels(bool sync);
        void add(Channel *channel);
		void add(const char*,const char*,const char*,const char*);
		void remove(const char*);
        void remove(Channel*);
		void add(const char*);
		void printList();
        void setAlert(const char*,const char*);
		Channel *find(const char*);
		int findPos(const char*);
        std::vector<Channel*> *getChannels(){ return &channels; }
		void clearData();
        void play(Channel*);
        Channel* getLastAdded();
        bool channelExists(const char*);
        void checkAllStreams();
        void parseOnlineStreams(std::string);

        bool parseChannelDataToJSON(std::string);
        int parseStreamDataToJSON(std::string);

        void checkResources();

    signals:
        void channelExists(Channel*);
        void channelNotFound(Channel*);
        void channelStateChanged(Channel*);
        void newChannel(Channel*);

};

#endif //CHANNEL_MANAGER_H
