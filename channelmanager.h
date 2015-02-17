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
#include "kstream/mainwindow.h"

#define TWITCH_URI "https://api.twitch.tv/kraken"

class ThreadManager;
class MainWindow;

class ChannelManager{

	protected:
		unsigned int update_counter, check_counter;
        std::vector<Channel*> channels;
        ThreadManager* tman;
        MainWindow *main;
        bool alive;
        bool threadsrunning;
	
	public:
		ChannelManager();
        ChannelManager(MainWindow*);

		~ChannelManager();	
        void load();
		bool readJSON(const char*);
		bool writeJSON(const char*);
		void checkStream(Channel*,bool);
		void check(Channel*,std::string);
        void checkStreams(bool sync);
		void update(Channel*);
		void update(Channel*,std::string);
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
        bool isAlive(){ return alive; }
        void updateGui();
        void finishThreads();
};

#endif //CHANNEL_MANAGER_H
