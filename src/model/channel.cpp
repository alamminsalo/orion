#include "channel.h"

Channel::~Channel(){
    //qDebug() << "Destroyer: Channel";
}


unsigned int Channel::getViewers() const
{
    return viewers;
}

void Channel::setViewers(unsigned int value)
{
    viewers = value;
}

const QString Channel::getGame() const
{
    return game;
}

void Channel::setGame(const QString &value)
{
    game = value;
}
Channel::Channel(){
    alert = true;
    timestamp = 0;
    online = false;
}

Channel::Channel(const QString &uri) : Channel(){
    this->uri = uri;
}

Channel::Channel(const QString &uri, const QString &name, const QString &info) : Channel(uri){
    this->name = name;
    this->info = info;
}

Channel::Channel(const QString &uri, const QString &name, const QString &info, bool alert) : Channel(name,uri,info){
    this->alert = alert;
}

Channel::Channel(const QString &uri, const QString &name, const QString &info, bool alert, time_t time) : Channel(name,uri,info,alert){
    this->timestamp = time;
}

Channel::Channel(const QString &uri, const QString &name, const QString &info, bool alert, time_t time, const QString &logo) : Channel(name,uri,info,alert,time){
    this->logouri = logo;
}

Channel::Channel(const QString &uri, const QString &name, const QString &info, bool alert, time_t time, const QString &logo, const QString &preview) : Channel(name,uri,info,alert,time,logo){
    this->previewuri = preview;
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

const QString Channel::lastOnline(){
    if (timestamp == 0){
        return "Never";
    }
    else{
        return QDateTime::fromMSecsSinceEpoch(timestamp).toString("MMM dd, hh:mm");
    }
}

const QJsonObject Channel::getJSON() const{
    QVariantMap map;
    map["title"]    = QVariant(name);
    map["uri"]      = QVariant(uri);
    map["info"]     = QVariant(info);
    map["logo"]     = QVariant(logouri);
    map["preview"]  = QVariant(previewuri);
    map["alert"]    = QVariant(alert);
    map["lastSeen"] = QVariant(timestamp);
    return QJsonObject::fromVariantMap(map);
    /*
    QString JSON_str = "{";
    JSON_str += "\"title\":\""+name+"\",";
    JSON_str += "\"uri\":\""+uri+"\",";
    JSON_str += "\"info\":\""+info+"\",";
    JSON_str += "\"alert\":\""+alert+"\",";
    JSON_str += "\"logo\":\""+logopath+"\",";
    JSON_str += "\"preview\":\""+previewpath+"\",";
    JSON_str += "\"lastSeen\":"+((quint32)timestamp);
	JSON_str += "}";

	return JSON_str;
    */
}

void Channel::setName(const QString &newName){
	name = newName;

}

void Channel::setURIName(const QString &newUri){
	uri = newUri;
}

void Channel::setInfo(const QString &newInfo){
	info = newInfo;
}

void Channel::setAlert(bool newAlert){
    alert = newAlert;
}

void Channel::updateTime(){
    timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
}

qint64 Channel::getTime(){
	return timestamp;
}

const QString Channel::getName(){
	return name;
}

const QString Channel::getUriName(){
	return uri;
}

const QString Channel::getFullUri(){
    return "http://twitch.tv/" + uri;
}

const QString Channel::getInfo(){
	return info;
}

bool Channel::hasAlert(){
    return alert;
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

const QString Channel::getLogoPath(){
    return logopath;
}

const QString Channel::getPreviewPath(){
    return previewpath;
}

void Channel::setLogoPath(const QString &path){
    logopath = path;
}

void Channel::setLogourl(const QString &uri){
    logouri = uri;
}

void Channel::setPreviewPath(const QString &path){
    previewpath = path;
}

const QString Channel::getLogourl(){
    return logouri;
}

void Channel::setPreviewurl(const QString &uri){
    previewuri = uri;
}

const QString Channel::getPreviewurl(){
    return previewuri;
}

void Channel::setLastSeen(time_t time){
    this->timestamp = time;
}

bool Channel::greaterThan (Channel* a, Channel* b) {
    if (a->isOnline() == b->isOnline()){ //BOTH ONLINE OR BOTH OFFLINE
        if (a->isOnline()){  //BOTH ONLINE, COMPARISON BY VIEWER COUNT
            return (a->getViewers() >= b->getViewers());
        }
        else{ //BOTH OFFLINE, COMPARISON BY DEFAULT QSTRING METHOD
            return (QString::compare(a->getName(),b->getName()) < 0);
        }
    }
    return a->isOnline();    //OTHER IS ONLINE AND OTHER IS NOT
}
