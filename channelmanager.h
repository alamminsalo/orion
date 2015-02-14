#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include "channel.h"
#include "fileutils.h"
#include "global.h"
#include "connector.h"
#include <rapidjson/document.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include "threadman.h"
//#include "tray-streamwatch/mainwindow.h"

#define TWITCH_URI "https://api.twitch.tv/kraken/"

class ThreadManager;
class MainWindow;

class ChannelManager{
	protected:
		unsigned int update_counter, check_counter;
		std::vector<Channel> channels;
		//Connector conn;
		ThreadManager* tman;
        //MainWindow *ui;
	
	public:
		ChannelManager();
        //ChannelManager(MainWindow*);
		~ChannelManager();	
		bool readJSON(const char*);
		bool writeJSON(const char*);
		void checkStream(Channel*,bool);
		void check(Channel*,std::string);
        void checkStreams(bool async);
		void update(Channel*);
		void update(Channel*,std::string);
		void updateChannels(bool);
		void add(Channel &channel);
		void add(const char*,const char*,const char*,const char*);
		void remove(const char*);
        void remove(Channel*);
		void removeAll();
		void add(const char*);
		void printList();
        void setAlert(const char*,const char*);
		Channel *find(const char*);
		int findPos(const char*);
		std::vector<Channel> *getChannels(){ return &channels; };
		void clearData();
};

#endif //CHANNEL_MANAGER_H
