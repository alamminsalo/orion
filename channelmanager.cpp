#include "channelmanager.h"
#include <QDir>
#include <QProcess>

ChannelManager::ChannelManager(){
    tman = new ThreadManager(this);
}

void ChannelManager::checkResources()
{
    if (!util::folderExists("resources")){
        std::cout << "dir \"resources\" not found, making...\n";
        QDir().mkdir("resources");
    }
    if (!util::fileExists("resources/icon.svg")){
        std::cout << "logo file not found, fetching...\n";
        conn::GetFile("https://raw.githubusercontent.com/alamminsalo/kstream/master/kstream/resources/icon.svg","resources/icon.svg");
    }
    if (!util::folderExists("resources/preview")){
        std::cout << "dir \"preview\" not found, making..\n";
        QDir().mkdir("resources/preview");
    }
    if (!util::fileExists("resources/preview/offline.png")){
        std::cout << "offline.png not found, fetching...\n";
        tman->getfile("https://raw.githubusercontent.com/alamminsalo/kstream/master/kstream/resources/preview/offline.png","resources/preview/offline.png");
    }
    if (!util::folderExists("resources/logos")){
        std::cout << "dir \"logos\" not found, making..\n";
        QDir().mkdir("resources/logos");
    }
    if (!util::folderExists("resources/scripts")){
        std::cout << "dir \"scripts\" not found, making..\n";
        QDir().mkdir("resources/scripts");
    }
    if (!util::fileExists("resources/scripts/play.sh")){
        conn::GetFile("https://raw.githubusercontent.com/alamminsalo/kstream/master/kstream/resources/scripts/play.sh","resources/scripts/play.sh");
#ifndef Q_OS_WIN
        QProcess::startDetached("chmod +x resources/scripts/play.sh");
#endif
    }
    if (!util::fileExists("resources/scripts/dialog.sh")){
        conn::GetFile("https://raw.githubusercontent.com/alamminsalo/kstream/master/kstream/resources/scripts/dialog.sh","resources/scripts/dialog.sh");
#ifndef Q_OS_WIN
        QProcess::startDetached("chmod +x resources/scripts/dialog.sh");
#endif
    }
    if (!util::fileExists("resources/logos/default.png")){
        std::cout << "default channel logo not found, fetching..." << std::endl;
        conn::GetFile(DEFAULT_LOGO_URL,"resources/logos/default.png");
    }
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
        //tman->check(channel);
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
        //tman->complete_threads(); //HALTS UNTIL COMPLETED! //TODO
		//std::cout << "\nThreads done.\n";
        //writeJSON(DATAURI);
	}
    else {
        tman->complete_threads_async();
    }
}

void ChannelManager::setAlert(const char* name, const char* val){
    Channel *channel = find(name);
    if (channel){
        channel->setAlert(val);
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
        if (channels[i] == channel){
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
    if (util::fileExists("resources/scripts/play.sh")){
        std::string cmd = "resources/scripts/play.sh "+channel->getFullUri();
        QProcess::startDetached(cmd.c_str());
    }
    else std::cout << "Couldn't locate 'play.sh'\n";
}

void ChannelManager::checkAllStreams()
{
    foreach (Channel *channel, channels){
        channel->setChanged(false);
    }
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
        parseStream(streams[i]);
    }

    //PARSE OFFLINE CHANNELS
    foreach (Channel *channel, channels){
        if (!channel->isChanged()){

            //If missing display name or channel logo data, update channel data
            if (channel->getName().empty() || !util::fileExists(channel->getLogoPath().c_str())){
                tman->update(channel);
            }

            if (channel->isOnline()){
                channel->setOnline(false);
                emit channelStateChanged(channel);
            }
        }
    }
}

int ChannelManager::parseStreamDataToJSON(std::string data){
    if (data.empty()){
        std::cout << "Error: empty data. Skipping update..\n";
        return 0;
    }

    rapidjson::Document doc;
    doc.Parse<0>(data.c_str());

    if (doc.HasParseError()){
        std::cout << "Error: " << data << "\n";
        return 0;
    }

    if (doc.HasMember("error") && doc.HasMember("status") && doc["status"].GetInt() == 404){ //Channel doesnt exist
        return -1;
    }

    if (!doc["stream"].IsNull()){
        parseStream(doc["stream"]);
        return 1;
    }
    else return 0;

}

void ChannelManager::parseStream(const rapidjson::Value& item){

    if (item["channel"].IsNull()){
        return;
    }
    Channel *channel = find(item["channel"]["name"].GetString());
    //std::cout << item["channel"]["name"].GetString() << std::endl;
    if (channel){

        if (!item["channel"]["display_name"].IsNull()){
            channel->setName(item["channel"]["display_name"].GetString());
        }

        if (!item["channel"]["logo"].IsNull()){
            std::string logouri = item["channel"]["logo"].GetString();
            std::string extension = logouri.substr(logouri.find_last_of("."));
            std::string logopath = "resources/logos/" + channel->getUriName() + extension;

            channel->setLogourl(logouri.c_str());
            channel->setLogoPath(logopath.c_str());

            if (!util::fileExists(logopath.c_str())){
                tman->getfile(logouri,logopath,channel);
            }
        }
        else {
            channel->setLogoPath("resources/logos/default.png");
        }

        if (!item["viewers"].IsNull()){
            channel->setViewers(item["viewers"].GetInt());
        }

        if (!item["game"].IsNull()){
            channel->setGame(item["game"].GetString());
        }

        if (!item["channel"]["status"].IsNull()){
            channel->setInfo(item["channel"]["status"].GetString());
        }

        if (channel->getPreviewurl().empty() && !item["preview"].IsNull()){
            if (!item["preview"]["medium"].IsNull()){
                //std::cout << "Fetching preview image...";
                std::string previewuri = item["preview"]["medium"].GetString();
                std::string extension = previewuri.substr(previewuri.find_last_of("."));
                std::string previewpath = "resources/preview/" + channel->getUriName() + extension;
                channel->setPreviewPath(previewpath.c_str());
                channel->setPreviewurl(previewuri.c_str());
                tman->getfile(previewuri,previewpath);
            }
        }

        if (!channel->isOnline()){
            channel->setOnline(true);
            emit channelStateChanged(channel);
        }
        else {
            channel->updated();
        }
        channel->setChanged(true);
    }
}

bool ChannelManager::parseChannelDataToJSON(std::string data){
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
        parseChannel(doc);
        return true;
    }
    return false;
}

void ChannelManager::parseChannel(const rapidjson::Document& item){
    if (item["name"].IsNull()){
        return;
    }
    Channel* channel = find(item["name"].GetString());

    if (channel){
        if (!item["display_name"].IsNull()){
            channel->setName(item["display_name"].GetString());
        }

        if (!item["status"].IsNull()){
            channel->setInfo(item["status"].GetString());
        }

        if (!item["logo"].IsNull()){
            std::string logouri = item["logo"].GetString();
            std::string extension = logouri.substr(logouri.find_last_of("."));
            std::string logopath = "resources/logos/" + channel->getUriName() + extension;


            channel->setLogourl(logouri.c_str());
            channel->setLogoPath(logopath.c_str());

            if (!util::fileExists(logopath.c_str())){
                tman->getfile(logouri,logopath,channel);
            }
        } else {
            channel->setLogoPath("resources/logos/default.png");
        }
        channel->updated();
    }
}
