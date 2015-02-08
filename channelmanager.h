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

class ChannelManager{
	protected:
		std::vector<Channel> channels;
		Connector conn;
		const char *TWITCH_URI;
		Channel *find(const char*);
		int findPos(const char*);
	
	public:
		ChannelManager();
		~ChannelManager(){};	
		bool readJSON(const char*);
		bool writeJSON(const char*);
		void checkStream(Channel*);
		void checkStreams();
		void update(Channel*);
		void updateChannels();
		void add(Channel &channel);
		void add(const char*,const char*,const char*,const char*);
		void remove(const char*);
		void removeAll();
		void add(const char*);
		void printList();
		void setAlert(const char*,bool);
};

#endif //CHANNEL_MANAGER_H
