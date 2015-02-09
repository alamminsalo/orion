#include "channelmanager.h"

ChannelManager::ChannelManager(){
}

bool ChannelManager::readJSON(const char *path){
	std::string str = util::readFile(path);	
	if (str == "") 
		return false;

	rapidjson::Document doc;
	doc.Parse(str.c_str());

	if (!doc.HasMember("channels")){
		std::cout<<"Error: Bad file format: Missing field \"channels\".\n";
			return false;
	}

	const rapidjson::Value &arr = doc["channels"];
	if (!arr.IsArray()){
		std::cout<<"Error: Bad file format: Not an array.\n";
		return false;
	}
	for (rapidjson::SizeType i=0; i < arr.Size(); ++i){
		Channel channel(
				arr[i]["title"].GetString(), 
				arr[i]["uri"].GetString(), 
				arr[i]["info"].GetString(),
				arr[i]["alert"].GetString(),
				arr[i]["lastSeen"].GetInt()
				);
		add(channel);
	}
	return true;
}

bool ChannelManager::writeJSON(const char *path){
	std::string output_str = "";
	output_str += "{\"channels\":[";
	for (unsigned int i=0; i<channels.size(); i++){
		output_str += channels.at(i).getJSON();
		if (i+1!=channels.size()) output_str += ",";
	}
	output_str += "]}";

	if (!util::writeFile(path,output_str))
		return true;
	return false;
}

void ChannelManager::add(Channel &channel){
	if (!find(channel.getUriName().c_str()))
		channels.push_back(channel);
}

void ChannelManager::add(const char* title, const char* uri, const char *info, const char *alert){
	Channel channel(title,uri,info,alert);
	add(channel);
}

void ChannelManager::add(const char* uriName){
	add("",uriName,"","on");
}

Channel *ChannelManager::find(const char* uriName){
	for (unsigned int i=0; i<channels.size(); i++){
		if (channels.at(i).getUriName() == uriName)
			return &channels.at(i);
	}
	//std::cout << "Couldn't find channel " << uriName << std::endl;
	return 0;
}

int ChannelManager::findPos(const char* uriName){
	for (unsigned int i=0; i<channels.size(); i++){
		if (channels.at(i).getUriName() == uriName)
			return i;
	}
	//std::cout << "Couldn't find channel " << uriName << std::endl;
	return -1;
}

void ChannelManager::updateChannels(){
	for (unsigned int i=0; i<channels.size(); i++){
		update(&channels.at(i));
	}
}

void ChannelManager::update(Channel *channel){
	if (channel){
		std::string uristr = TWITCH_URI;
		uristr += "/channels/";
		uristr += channel->getUriName();

		std::string resp = conn.Get(uristr.c_str());

		if (resp != ""){
			rapidjson::Document doc;
			doc.Parse(resp.c_str());
			//assert(doc.IsObject());
			
			if (!doc.HasMember("error")){
				channel->setName(doc["display_name"].GetString());
				channel->setInfo(doc["status"].GetString());

				std::string cmdstr = "./fetchlogo " + channel->getUriName() + " " + doc["logo"].GetString();	
				system(cmdstr.c_str());
			}
			else channel->setInfo("Channel not found.");
		}
	}
	else std::cout << "Error: Channel is null\n";
}

void ChannelManager::setAlert(const char* name, bool val){
	Channel *channel = find(name);
	if (channel){
		if (val)
			find(name)->enableAlert();
		else
		   	find(name)->disableAlert();
	}
}

void ChannelManager::checkStream(Channel *channel){
	if (!channel){
		std::cout << "Error: Channel* is NULL\n";
		return;
	}
	if (channel->hasAlert()){
		rapidjson::Document doc;
		std::string uristr;

		if (channel->getName().empty() || channel->getInfo().empty()){
			std::cout << "Fetching data for new channel...\n";
			update(channel);
			writeJSON(DATAURI);
		}

		std::string name = channel->getUriName();

		std::cout<<"Checking channel "<< name <<"...";

		uristr = TWITCH_URI;
		uristr += "/streams/";
		uristr += name;

		std::string response = conn.Get(uristr.c_str());

		doc.Parse(response.c_str());
		//assert(doc.IsObject());

		if (!doc.HasMember("error")){
			if (doc["stream"].IsNull()){
				std::cout<<"offline\n";
				if (channel->isOnline()){
					std::string cmdstr = "./dialog.sh \"" + channel->getUriName() + "\" \"" + channel->getName() + "\" \"" + channel->getInfo() + "\" off";
					system(cmdstr.c_str());
					channel->setOnline(false);
				}
			}
			else {
				std::cout<<"online\n";
				if (!channel->isOnline()){
					std::string cmdstr = "./dialog.sh \"" + channel->getUriName() + "\" \"" + channel->getName() + "\" \"" + channel->getInfo() + "\" on";
					system(cmdstr.c_str());
					channel->setOnline(true);
				}
				channel->updateTime();
			}
		}
		else std::cout << "not found\n";
	}
}

void ChannelManager::checkStreams(){
	std::cout << "Checking all streams..\n";
	for (unsigned int i=0; i<channels.size(); i++)
		checkStream(&channels.at(i));
	std::cout << "Done checking\n";
}

void ChannelManager::printList(){
	std::cout<<"\n";
	for (unsigned int i=0; i<channels.size(); i++){
		Channel *channel = &channels.at(i);
		std::cout << "Name: " << channel->getName() << std::endl;
		std::cout << "Info: " << channel->getInfo() << std::endl;
		std::cout << "URL: " << channel->getFullUri() << std::endl;
		std::cout << "Id: " << channel->getUriName() << std::endl;
		std::cout << "Notification: " << (channel->hasAlert() ? "On" : "Off") << std::endl;
		std::cout << "Last online: " << channel->lastOnline() << std::endl;
		std::cout << std::endl;
	}
}

void ChannelManager::remove(const char *channelName){	
	int i = findPos(channelName);
	if (i != -1)
		channels.erase(channels.begin() + i);
}

void ChannelManager::removeAll(){
	channels.clear();
}
