#include "channel.h"

Channel::Channel(){
	name = uri = info = "";
	alert = "on";
	timestamp = time(0);
	online = false;
}

Channel::Channel(const char *newName, const char *newUri, const char *newInfo){
	name = newName;
	uri = newUri;
	info = newInfo;
	alert = "on";
	online = false;
}

Channel::Channel(const char *newName, const char *newUri, const char *newInfo, const char *newAlert){
	name = newName;
	uri = newUri;
	info = newInfo;
	alert = newAlert;
	online = false;
}

Channel::Channel(const char *newName, const char *newUri, const char *newInfo, const char *newAlert, time_t newTime){
	name = newName;
	uri = newUri;
	info = newInfo;
	alert = newAlert;
	timestamp = newTime;
	online = false;
}

Channel::Channel(const Channel &channel){
	this->name = channel.name;
	this->uri = channel.uri;
	this->info = channel.info;
	this->alert = channel.alert;
	this->timestamp = channel.timestamp;
	this->online = channel.online;
}

std::string Channel::lastOnline(){
	if (timestamp == 0)
		return "Never";
	std::string ret = ctime(&timestamp);
	return ret.substr(0,ret.find("\n"));
}

std::string Channel::getJSON(){
	std::string JSON_str = "{";
	JSON_str += "\"title\":\""+this->name+"\",";
	JSON_str += "\"uri\":\""+this->uri+"\",";
	JSON_str += "\"info\":\""+this->info+"\",";
	JSON_str += "\"alert\":\""+this->alert+"\",";
	JSON_str += "\"lastSeen\":"+std::to_string((unsigned long)this->timestamp);
	JSON_str += "}";
	return JSON_str;
}
