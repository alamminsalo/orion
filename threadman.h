#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "channelmanager.h"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <vector>

class ChannelManager;

class ThreadManager{

	std::vector<std::thread> threads;
    ChannelManager *cman;
    bool threadsrunning;
    bool polling;

	public:
		ThreadManager(ChannelManager*);		
		~ThreadManager();		

		void check(Channel*);
        void checkAll();
		void update(Channel*);
        void getfile(std::string,std::string,Channel*);

        void startPolling();
        bool isPolling();

		void complete_threads();
        void complete_threads_async();
        void wait_for_threads();
        void finish_threads();
};

void t_update(Channel*, ChannelManager*);
void t_check(Channel*, ChannelManager*);
void t_checkAll(ChannelManager*);
void t_poll(ThreadManager*);
void t_getfile(std::string,std::string,Channel*);
void t_complete_async(std::vector<std::thread>*, ThreadManager*);

#endif
