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
#include <iomanip>

#define TWITCH_URI "https://api.twitch.tv/kraken/"

class ChannelManager{
	protected:
		std::vector<Channel> channels;
		Connector conn;
	
	public:
		ChannelManager();
		~ChannelManager(){};	
		bool readJSON(const char*);
		bool writeJSON(const char*);
		void checkStream(Channel*,bool);
		void checkStreams(bool);
		void update(Channel*);
		void updateChannels();
		void add(Channel &channel);
		void add(const char*,const char*,const char*,const char*);
		void remove(const char*);
		void removeAll();
		void add(const char*);
		void printList();
		void setAlert(const char*,bool);
		Channel *find(const char*);
		int findPos(const char*);
};

#endif //CHANNEL_MANAGER_H
