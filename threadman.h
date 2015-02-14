#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "channelmanager.h"
#include <iostream>
#include <thread>
#include <unistd.h>

class ChannelManager;

static ChannelManager *cman_ptr;

void t_update(Channel*);
void t_check(Channel*);

struct FileData{
	std::string uri;
	std::string path;
	FileData(std::string u, std::string p){
		uri = u;
		path = p;
	}
};
void t_getfile(std::string,std::string);

class ThreadManager{
	std::vector<std::thread> threads;

	public:
		ThreadManager(){};
		ThreadManager(ChannelManager*);		
		~ThreadManager();		

		void check(Channel*);
		void update(Channel*);
		void getfile(std::string,std::string);

		void complete_threads();
        void complete_threads_async();
};

#endif
