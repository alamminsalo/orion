#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "channelmanager.h"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <vector>

class ChannelManager;

//static ChannelManager *cman_ptr;

void t_update(Channel*, ChannelManager*);
void t_check(Channel*, ChannelManager*);
void t_poll(ChannelManager*);
void t_getfile(std::string,std::string);

class ThreadManager{

	std::vector<std::thread> threads;
    ChannelManager *cman;

	public:
		ThreadManager(ChannelManager*);		
		~ThreadManager();		

		void check(Channel*);
		void update(Channel*);
		void getfile(std::string,std::string);

        void startPolling();

		void complete_threads();
        void complete_threads_async();
};

#endif
