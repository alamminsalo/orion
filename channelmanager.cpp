#include "channelmanager.h"

ChannelManager::ChannelManager(){
    tman = new ThreadManager(this);
}

ChannelManager::~ChannelManager(){
    std::cout << "Destroyer: ChannelManager\n";
    delete tman;
    save();
    clearData();
}

void ChannelManager::load(){
    readJSON(DATAURI);
}

void ChannelManager::save()
{
    writeJSON(DATAURI);
}

bool ChannelManager::readJSON(const char *path){
	std::string str = util::readFile(path);	
    if (str.empty())
		return false;

	rapidjson::Document doc;
    doc.Parse<0>(str.c_str());


    if (!doc.IsObject())
        std::cout << "Error in \"./data.json\": malformed JSON\n";
    assert(doc.IsObject());


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

        if (!arr[i].HasMember("title"))
            std::cout << "title missing\n";
        if (!arr[i].HasMember("uri"))
            std::cout << "uri missing\n";
        if (!arr[i].HasMember("info"))
            std::cout << "info missing\n";
        if (!arr[i].HasMember("alert"))
            std::cout << "alert missing\n";
        if (!arr[i].HasMember("lastSeen"))
            std::cout << "lastSeen missing\n";
        if (!arr[i].HasMember("logo"))
            std::cout << "logo missing\n";
        if (!arr[i].HasMember("preview"))
            std::cout << "preview missing\n";

        /*std::cout << arr[i]["title"].GetString() << std::endl;//Check that these fields exist
        std::cout << arr[i]["uri"].GetString() << std::endl;
        std::cout << arr[i]["info"].GetString() << std::endl;
        std::cout <<  arr[i]["alert"].GetString() << std::endl;
        std::cout << arr[i]["lastSeen"].GetInt() << std::endl;
        std::cout << arr[i]["logo"].GetString() << std::endl;*/

        channels.push_back(new Channel(
            arr[i]["title"].GetString(),
            arr[i]["uri"].GetString(),
            arr[i]["info"].GetString(),
            arr[i]["alert"].GetString(),
            arr[i]["lastSeen"].GetInt(),
            arr[i]["logo"].GetString(),
            arr[i]["preview"].GetString()
        ));
	}
	return true;
}

bool ChannelManager::writeJSON(const char *path){
	std::string output_str = "";
	output_str += "{\"channels\":[";
    for (unsigned int i=0; i < channels.size(); i++){
        output_str += channels.at(i)->getJSON() + (i+1 < channels.size() ? ",":"");
	}
	output_str += "]}";

	if (!util::writeFile(path,output_str))
		return true;
	return false;
}

void ChannelManager::add(Channel *channel){
    channels.push_back(channel);
    emit newChannel(channel);
}

void ChannelManager::add(const char* title, const char* uri, const char *info, const char *alert){
    add(new Channel(title,uri,info,alert));
}

void ChannelManager::add(const char* uriName){
    Channel *channel = find(uriName);
    if (channel){
        emit channelExists(channel);
    }
    else{
        channel = new Channel(uriName);
        add(channel);
        tman->add(channel);
        tman->check(channel);
    }
}

Channel *ChannelManager::find(const char* uriName){
	for (unsigned int i=0; i<channels.size(); i++){
        if (channels.at(i)->getUriName() == uriName)
            return channels.at(i);
	}
	//std::cout << "Couldn't find channel " << uriName << std::endl;
	return 0;
}

int ChannelManager::findPos(const char* uriName){
	for (unsigned int i=0; i<channels.size(); i++){
        if (channels.at(i)->getUriName() == uriName)
			return i;
	}
	//std::cout << "Couldn't find channel " << uriName << std::endl;
	return -1;
}

void ChannelManager::updateChannels(bool sync){
    //readJSON(DATAURI);
	for (unsigned int i=0; i<channels.size(); i++){
        tman->update(channels.at(i));
	}
	if (sync){
		//std::cout << "\nWaiting threads...\n";
		tman->complete_threads(); //HALTS UNTIL COMPLETED!
		//std::cout << "\nThreads done.\n";
        //writeJSON(DATAURI);
	}
    else {
        tman->complete_threads_async();
    }
}

bool ChannelManager::update(Channel *channel){
	std::string uristr = TWITCH_URI;
	uristr += "/channels/";
	uristr += channel->getUriName();
    return update(channel,conn::Get(uristr).c_str());
}

bool ChannelManager::update(Channel *channel, std::string data){
	if (data.empty()){
		std::cout << "Error: empty data. Skipping update..\n";
        return false;
	}

	rapidjson::Document doc;
    doc.Parse<0>(data.c_str());
	
    if (!doc.IsObject()){
        std::cout << "Error: " << data << "\n";
        return false;
    }

	if (!doc.HasMember("error")){

        if (doc.HasMember("display_name")){
			const rapidjson::Value& name = doc["display_name"];
			if (!name.IsNull())
				channel->setName(doc["display_name"].GetString());
			else channel->setName(channel->getUriName().c_str());
		}

        if (doc.HasMember("status")){
			const rapidjson::Value& status = doc["status"];
			if (!status.IsNull())
				channel->setInfo(doc["status"].GetString());
			else channel->setInfo("No status found");
		}

        if (!util::folderExists("logos")){
			std::cout << "dir \"logos\" not found, making..\n";
			system("mkdir logos");
		}


        if (doc.HasMember("logo") && !doc["logo"].IsNull()){

				std::string logouri = doc["logo"].GetString();
				std::string extension = logouri.substr(logouri.find_last_of("."));
				std::string logopath = "logos/" + channel->getUriName() + extension;


                channel->setLogourl(logouri.c_str());

                if (!util::fileExists(logopath.c_str())){
                    //std::cout << "Fetching logo...";
                    //conn::GetFile(logouri,logopath);
                    //channel->setLogoPath(logopath.c_str());
                    //channel->updated();
                conn::GetFile(logouri,logopath);

                    //std::cout << "Done.\n";
                }
                channel->setLogoPath(logopath.c_str());

		}
        else{
            channel->setLogoPath("logos/default.png");
        }
        channel->updated();
        //tman->check(channel);
        //checkStream(channel,false);
        return true;
	}
    return false;
}

void ChannelManager::setAlert(const char* name, const char* val){
    Channel *channel = find(name);
    if (channel){
        channel->setAlert(val);
	}
}

void ChannelManager::check(Channel *channel, std::string data){
	if (data.empty()){
		std::cout << "Error: empty data. Skipping update..\n";
		return;
	}

	rapidjson::Document doc;
    doc.Parse<0>(data.c_str());

    if (!doc.IsObject()){
        std::cout << "Error: " << data << "\n";
		return;
    }
	
    //std::string logopath = "./logos/" + channel->getUriName();
    //if (channel->getName().empty() || channel->getInfo().empty() || !util::fileExists(logopath.c_str())){
      //  if (!update(channel)) return;
    //}

	if (channel->hasAlert()){

		if (!doc.HasMember("error")){
			if (doc["stream"].IsNull()){
                if (channel->isOnline()){
                    //std::string cmdstr = "./dialog.sh \"" + channel->getUriName() + "\" \"" + channel->getName() + "\" \"" + channel->getInfo() + "\" off";
                    //system(cmdstr.c_str());
					channel->setOnline(false);
                    emit channelStateChanged(channel);
                }
			}
			else {
				if (!channel->isOnline()){
                    //std::string cmdstr = "./dialog.sh \"" + channel->getUriName() + "\" \"" + channel->getName() + "\" \"" + channel->getInfo() + "\" on";
                    //system(cmdstr.c_str());
					channel->setOnline(true);

                    if (!util::folderExists("preview")){
                        std::cout << "dir \"preview\" not found, making..\n";
                        system("mkdir preview");
                    }
                    emit channelStateChanged(channel);

				}
                if (!doc["stream"]["viewers"].IsNull()){
                    channel->setViewers(doc["stream"]["viewers"].GetInt());
                }
                if (!doc["stream"]["preview"].IsNull()){
                    if (!doc["stream"]["preview"]["medium"].IsNull()){
                        std::cout << "Fetching preview image...";
                        std::string previewuri = doc["stream"]["preview"]["medium"].GetString();
                        std::string extension = previewuri.substr(previewuri.find_last_of("."));
                        std::string previewpath = "preview/" + channel->getUriName() + extension;
                        channel->setPreviewPath(previewpath.c_str());
                        channel->setPreviewurl(previewuri.c_str());
                        conn::GetFile(previewuri,previewpath);
                        std::cout << "Done.\n";
                    }
                }
			}
            channel->updated();
		}
    }

}

void ChannelManager::checkStream(Channel *channel, bool sync){
    tman->check(channel);
    if (sync){
        tman->complete_threads();
        //writeJSON(DATAURI);
    }
    else {
        tman->complete_threads_async();
    }
}

void ChannelManager::checkStreams(bool sync){
	for (unsigned int i=0; i<channels.size(); i++){
        tman->check(channels.at(i));
	}
    if (sync){
        tman->complete_threads(); //HELP
        //writeJSON(DATAURI);
	}
    else {
        tman->complete_threads_async();
    }
}

void ChannelManager::printList(){
	std::cout<<"\n";
	for (unsigned int i=0; i<channels.size(); i++){
        Channel *channel = channels.at(i);
		std::cout << "Name: " << channel->getName() << std::endl;
		std::cout << "Info: " << channel->getInfo() << std::endl;
		std::cout << "URL: " << channel->getFullUri() << std::endl;
		std::cout << "Id: " << channel->getUriName() << std::endl;
		std::cout << "Notification: " << (channel->hasAlert() ? "On" : "Off") << std::endl;
		std::cout << "Last online: " << channel->lastOnline() << std::endl;
        std::cout << "Logo: " << channel->getLogoPath() << std::endl;
		std::cout << std::endl;
	}
}

void ChannelManager::remove(const char *channelName){	
	int i = findPos(channelName);
    if (i != -1){
        delete channels.at(i);
		channels.erase(channels.begin() + i);
    }
}

void ChannelManager::remove(Channel* channel){
    tman->wait_for_threads();
    for (unsigned int i=0; i < channels.size(); i++){
        if (channels.at(i) == channel){
            delete channel;
            channels.erase(channels.begin() + i);
        }
    }
}

void ChannelManager::clearData(){
    for (unsigned int i = 0; i < channels.size(); i++){
        delete channels.at(i);
	}
    channels.clear();
}

void ChannelManager::play(Channel* channel){
    if (util::fileExists("play.sh")){
        std::string cmd = "./play.sh "+channel->getFullUri()+" &";
        std::cout << cmd << "\n";
        system(cmd.c_str());
    }
    else std::cout << "Couldn't locate 'play.sh'\n";
}

void ChannelManager::checkAllStreams()
{
    foreach (Channel *channel, channels)
        channel->setChanged(false);
    if (channels.size() > 0)
        tman->checkAll();
}

void ChannelManager::parseOnlineStreams(std::string data)
{
    rapidjson::Document doc;
    doc.Parse<0>(data.c_str());

    if (doc.HasParseError() || !doc.IsObject())
        return;

    const rapidjson::Value& streams = doc["streams"];

    //std::cout << streams.Size();

// PARSE ONLINE CHANNELS
    for (rapidjson::SizeType i=0; i < streams.Size(); i++){
        const rapidjson::Value& item = streams[i];
        Channel *channel = find(item["channel"]["name"].GetString());
        //std::cout << item["channel"]["name"].GetString() << std::endl;
        if (!channel)
            continue;

        if (!util::folderExists("preview")){
            std::cout << "dir \"preview\" not found, making..\n";
            system("mkdir preview");
        }

        if (!item["viewers"].IsNull()){
            channel->setViewers(item["viewers"].GetInt());
            std::cout << channel->getViewers() << std::endl;
        }

        if (channel->getPreviewurl().empty() && !item["preview"].IsNull()){
            if (!item["preview"]["medium"].IsNull()){
                std::cout << "Fetching preview image...";
                std::string previewuri = item["preview"]["medium"].GetString();
                std::string extension = previewuri.substr(previewuri.find_last_of("."));
                std::string previewpath = "preview/" + channel->getUriName() + extension;
                channel->setPreviewPath(previewpath.c_str());
                channel->setPreviewurl(previewuri.c_str());
                tman->getfile(previewuri,previewpath);
            }
        }

        if (!channel->isOnline()){
            channel->setOnline(true);
            emit channelStateChanged(channel);
        }
        channel->setChanged(true);
    }

//PARSE OFFLINE CHANNELS
    foreach (Channel *channel, channels){
        if (!channel->isChanged() && channel->isOnline()){
            channel->setOnline(false);
            emit channelStateChanged(channel);
        }
    }
}
