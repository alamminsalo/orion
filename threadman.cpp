#include "threadman.h"

ThreadManager::ThreadManager(ChannelManager *c){
    cman = c;
}

ThreadManager::~ ThreadManager(){
    std::cout << "Destroyer: ThreadManager\n";
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

void t_poll(ChannelManager *cman){
    while (cman->isAlive()){
        cman->checkStreams(true);
        for (int i=0; i < 30 && cman->isAlive(); i++)
            sleep(1);
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

void complete_async(std::vector<std::thread> *threads,ChannelManager *cman){
    try{
        for (unsigned int i=0; i < threads->size(); i++){
                threads->at(i).join();
        }

        threads->clear();
    } catch(std::exception& e){
        std::cout << e.what();
    }
    cman->writeJSON(DATAURI);
    cman->finishThreads();
}

void ThreadManager::complete_threads_async(){
    std::thread t(complete_async,&threads,cman);
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
    threads.push_back(std::thread(t_poll,cman));
}

