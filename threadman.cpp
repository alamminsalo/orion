#include "threadman.h"

ThreadManager::ThreadManager(ChannelManager *c){
    cman = c;
    polling = false;
}

ThreadManager::~ ThreadManager(){
    std::cout << "Destroyer: ThreadManager\n";
    polling = false;
    this->wait_for_threads();
    this->complete_threads();
}

void t_check(Channel *channel, ChannelManager *cman){
	if (channel){
		std::string uristr = TWITCH_URI;
		uristr += "/streams/";
		uristr += channel->getUriName();

		std::string str = conn::Get(uristr.c_str());	

        cman->check(channel,str);
	}
}

void t_update(Channel *channel, ChannelManager *cman){
	if (channel){
		std::string uristr = TWITCH_URI;
		uristr += "/channels/";
		uristr += channel->getUriName();
		std::string str = conn::Get(uristr.c_str());	

        cman->update(channel,str);
	}
}

void t_getfile(std::string uri, std::string path){
	std::cout << "uri: " << uri << " path: " << path << "\n";
	if (uri.empty()){
		std::cout << "No url set for file!\n";
		return;
	}
	if (path.empty()){
		std::cout << "No url set for file!\n";
		return;
	}
	conn::GetFile(uri.c_str(),path.c_str());
}

void t_poll(ThreadManager *tman){
    while (tman->isPolling()){
        //cman->checkStreams(true);
        for (int i=0; i < 30 && tman->isPolling(); i++)
            usleep(1000);
    }
}

void ThreadManager::complete_threads(){
    try{
        for (unsigned int i=0; i < threads.size(); i++){
            threads.at(i).join();
        }

        threads.clear();

    } catch(std::exception& e){
        std::cout << "\nError: "<< e.what() << "\n";
    }
}

void t_complete_async(std::vector<std::thread> *threads,ThreadManager *tman){
    try{
        for (unsigned int i=0; i < threads->size(); i++){
                threads->at(i).join();
        }

        threads->clear();
    } catch(std::exception& e){
        std::cout << e.what();
    }

    tman->finish_threads();
}

void ThreadManager::complete_threads_async(){
    threadsrunning = true;
    std::thread t(t_complete_async,&threads,this);
    t.detach();
}

void ThreadManager::update(Channel *channel){
    threads.push_back(std::thread(t_update,channel,cman));
}

void ThreadManager::check(Channel *channel){
    threads.push_back(std::thread(t_check,channel,cman));
}

void ThreadManager::getfile(std::string uri, std::string path){
	threads.push_back(std::thread(t_getfile,uri,path));
}

void ThreadManager::startPolling(){
    polling = true;
    threads.push_back(std::thread(t_poll,this));
}

bool ThreadManager::isPolling()
{
    return polling;
}

void ThreadManager::finish_threads(){
    cman->save();
    threadsrunning = false;
}

void ThreadManager::wait_for_threads(){
    std::cout << "Waiting for threads to finish..\n";
    while (threadsrunning) usleep(500);
}
