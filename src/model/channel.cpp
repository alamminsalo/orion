#include "channel.h"

Channel::~Channel(){
    //qDebug() << "Destroyer: Channel";
}

void Channel::updateWith(const Channel &other)
{
    if (this->getId() == other.getId()) {
        this->setName(other.getName());
        this->setLogourl(other.getLogourl());
        this->setInfo(other.getInfo());
        this->setViewers(other.getViewers());
        this->setGame(other.getGame());
    }
}

quint32 Channel::getId() const
{
    return id;
}

void Channel::setId(const quint32 &value)
{
    id = value;
}

bool Channel::isFavourite() const
{
    return favourite;
}

void Channel::setFavourite(bool value)
{
    favourite = value;
}

Channel::Channel(){
    alert = true;
    timestamp = 0;
    online = false;
    viewers = 0;
    id = 0;
    favourite = false;
}

Channel::Channel(const QString &uri) : Channel(){
    this->serviceName = uri;
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
    this->id = channel.id;
	this->name = channel.name;
    this->serviceName = channel.serviceName;
	this->info = channel.info;
	this->alert = channel.alert;
	this->timestamp = channel.timestamp;
	this->online = channel.online;
    this->logouri = channel.logouri;
    this->previewuri = channel.previewuri;
    this->game = channel.game;
    this->viewers = channel.viewers;
    this->favourite = channel.favourite;
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
    map["uri"]      = QVariant(serviceName);
    map["info"]     = QVariant(info);
    map["logo"]     = QVariant(logouri);
    map["preview"]  = QVariant(previewuri);
    map["alert"]    = QVariant(alert);
    map["lastSeen"] = QVariant(timestamp);
    map["id"]       = QVariant(id);
    return QJsonObject::fromVariantMap(map);
}

void Channel::setName(const QString &newName){
	name = newName;

}

void Channel::setServiceName(const QString &newUri){
    serviceName = newUri;
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

const QString Channel::getName() const{
	return name;
}

const QString Channel::getServiceName(){
    return serviceName;
}

const QString Channel::getFullUri(){
    return "http://twitch.tv/" + serviceName;
}

const QString Channel::getInfo() const{
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

void Channel::setLogourl(const QString &uri){
    logouri = uri;
}

const QString Channel::getLogourl() const{
    return !logouri.isEmpty() ? logouri : DEFAULT_LOGO_URL;
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

qint32 Channel::getViewers() const
{
    return online ? viewers : -1;
}

void Channel::setViewers(qint32 value)
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
