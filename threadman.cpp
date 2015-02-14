#include "threadman.h"

ThreadManager::ThreadManager(ChannelManager *c){
	cman_ptr = c; 
}

ThreadManager::~ThreadManager(){
	for (size_t i=0; i<threads.size(); i++)
		threads.at(i).join();
	std::cout << "Finished threadmanager.\n";
}

void t_check(Channel *channel){
	if (channel){
		std::string uristr = TWITCH_URI;
		uristr += "/streams/";
		uristr += channel->getUriName();
		//Connector conn;
		std::string str = conn::Get(uristr.c_str());	

		cman_ptr->check(channel,str);
	}
}

void t_update(Channel *channel){
	if (channel){
		std::string uristr = TWITCH_URI;
		uristr += "/channels/";
		uristr += channel->getUriName();
		//Connector conn;
		std::string str = conn::Get(uristr.c_str());	

		cman_ptr->update(channel,str);
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

void ThreadManager::complete_threads(){
	try{
		for (size_t i=0; i < threads.size(); i++){
				threads.at(i).join();
		}

		threads.clear();

	} catch(std::exception& e){
		std::cout << e.what();
	}
}

void complete_async(std::vector<std::thread> *threads){
    try{
        for (size_t i=0; i < threads->size(); i++){
                threads->at(i).join();
        }

        threads->clear();

    } catch(std::exception& e){
        std::cout << e.what();
    }
	cman_ptr->writeJSON(DATAURI);
}

void ThreadManager::complete_threads_async(){
    std::thread t(complete_async,&threads);
    t.detach();
}

void ThreadManager::update(Channel *channel){
	threads.push_back(std::thread(t_update,channel));
}

void ThreadManager::check(Channel *channel){
	threads.push_back(std::thread(t_check,channel));
}

void ThreadManager::getfile(std::string uri, std::string path){
	threads.push_back(std::thread(t_getfile,uri,path));
}

