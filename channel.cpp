#include "channel.h"

Channel::~Channel(){
    std::cout << "Destroyer: Channel\n";
}


unsigned int Channel::getViewers() const
{
    return viewers;
}

void Channel::setViewers(unsigned int value)
{
    viewers = value;
}

std::string Channel::getGame() const
{
    return game;
}

void Channel::setGame(const std::string &value)
{
    game = value;
}
Channel::Channel(){
    alert = "on";
    timestamp = time(0);
    online = false;
    //changed = true;
}

Channel::Channel(const char* uri) : Channel(){
    this->uri = uri;
}

Channel::Channel(const char *name, const char *uri, const char *info) : Channel(uri){
    this->name = name;
    this->info = info;
}

Channel::Channel(const char *name, const char *uri, const char *info, const char *alert) : Channel(name,uri,info){
    this->alert = alert;
}

Channel::Channel(const char *name, const char *uri, const char *info, const char *alert, time_t time) : Channel(name,uri,info,alert){
    this->timestamp = time;
}

Channel::Channel(const char *name, const char *uri, const char *info, const char *alert, time_t time, const char* logo) : Channel(name,uri,info,alert,time){
    this->logopath = logo;
}

Channel::Channel(const char *name, const char *uri, const char *info, const char *alert, time_t time, const char* logo, const char* preview) : Channel(name,uri,info,alert,time,logo){
    this->previewpath = preview;
}

Channel::Channel(const Channel &channel){
	this->name = channel.name;
	this->uri = channel.uri;
	this->info = channel.info;
	this->alert = channel.alert;
	this->timestamp = channel.timestamp;
	this->online = channel.online;
    this->logopath = channel.logopath;
    this->previewpath = channel.previewpath;
    emit updated();
}

std::string Channel::lastOnline(){
    std::string date;
    if (timestamp == 0){
        date = "Never";
    }
    else{
        date = ctime(&timestamp);
        date = date.substr(0,date.find("\n"));
    }
    return date;
}

const std::string Channel::getJSON() const{
	std::string JSON_str = "{";
    JSON_str += "\"title\":\""+name+"\",";
    JSON_str += "\"uri\":\""+uri+"\",";
    JSON_str += "\"info\":\""+info+"\",";
    JSON_str += "\"alert\":\""+alert+"\",";
    JSON_str += "\"logo\":\""+logopath+"\",";
    JSON_str += "\"preview\":\""+previewpath+"\",";
    JSON_str += "\"lastSeen\":"+std::to_string((unsigned long)timestamp);
	JSON_str += "}";

	return JSON_str;
}

void Channel::setName(const char *newName){
	name = newName;

}

void Channel::setURIName(const char *newUri){
	uri = newUri;
}

void Channel::setInfo(const char *newInfo){
	info = newInfo;
}

void Channel::setAlert(const char* newAlert){
    alert = newAlert;
}

void Channel::updateTime(){
	timestamp = time(0);
}

time_t Channel::getTime(){
	return timestamp;
}

std::string Channel::getName(){
	return name;
}

std::string Channel::getUriName(){
	return uri;
}

std::string Channel::getFullUri(){
    return "http://twitch.tv/" + uri;
}

std::string Channel::getInfo(){
	return info;
}

bool Channel::hasAlert(){
	return (alert == "on" ? true : false);
}


void Channel::setOnline(bool b){
	online = b;
    if (online)
        updateTime();
    emit updated();
}

bool Channel::isOnline(){
	return online;
}

void Channel::clear(){
	name.clear();
	info.clear();
    logopath.clear();
    previewpath.clear();
}

std::string Channel::getLogoPath(){
    return logopath;
}

std::string Channel::getPreviewPath(){
    return util::fileExists(previewpath.c_str()) ? previewpath : "resources/preview/offline.png";
}

bool Channel::isChanged()
{
    return changed;
}

void Channel::setChanged(bool b){
    changed = b;
}

void Channel::setLogoPath(const char *path){
    logopath = path;
    emit iconUpdated();
}

void Channel::setLogourl(const char *uri){
    logouri = uri;
}

void Channel::setPreviewPath(const char *path){
    previewpath = path;
}

std::string Channel::getLogourl(){
    return logouri;
}

void Channel::setPreviewurl(const char *uri){
    previewuri = uri;
}

std::string Channel::getPreviewurl(){
    return previewuri;
}

void Channel::setLastSeen(time_t time){
    this->timestamp = time;
}

bool Channel::isEmpty(){
    return (name.empty() || info.empty()) ? true : false;
}
