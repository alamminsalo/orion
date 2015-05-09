#include "streamitem.h"

StreamItem::StreamItem(Channel* channel)
{
    this->channel = channel;
}
StreamItem::~StreamItem(){
    qDebug() << "Destroyer: StreamItem";
}


void StreamItem::update(){

    //if (channel->hasChanged()){     //To reduce some useless updating

        if (this->icon().pixmap(QSize(32,32)).isNull()){ //MISSING ICON
            setIcon(QIcon(channel->getLogoPath().c_str()));
        }
        //if (this->text().isEmpty() && !getName().isEmpty()){
        QString title = getName();

        //}
        QString image = "<img src=\"";
        image += channel->getPreviewPath().c_str();
        image += "\"></img>";
        QString tooltip = "<table><tr><td>"+getInfo()+"</td></tr>";
        tooltip += "<tr><td>"+image+"</td></tr>";

        if (!online()){
            tooltip += "<tr><td>Last seen: ";
            tooltip += this->getChannel()->lastOnline().c_str();
            tooltip += "</td></tr>";
        }
        else {
            QString viewercount = std::to_string(channel->getViewers()).c_str();
            tooltip += "<tr><td><b>Now streaming:</b> <i>"+viewercount+" viewers</i></td></tr>";
        }

        this->setText(title);

        tooltip += "</table>";
        QListWidgetItem::setToolTip(tooltip);

        if (this->online()){
            this->setTextColor(QColor(0, 0, 0));
        }
        else this->setTextColor(QColor(200, 200, 200));
}

Channel* StreamItem::getChannel() const{
    return channel;
}

bool StreamItem::online() const{
    return channel->isOnline();
}

const QString StreamItem::getName(){
    return channel->getName().c_str();
}

const QString StreamItem::getUriName(){
    return channel->getUriName().c_str();
}

const QString StreamItem::getInfo(){
    return channel->getInfo().c_str();
}

bool StreamItem::operator< (const QListWidgetItem& other) const{
    const StreamItem& stritem = dynamic_cast<const StreamItem&>(other);
    //return (this->online() != stritem.online()) ? this->online() : (QString::compare(this->text(),stritem.text()) < 0);
    //return (this->online() != stritem.online()) ? this->online() : (channel->getViewers() >= stritem.getChannel()->getViewers());

    if (online() == stritem.online()){ //BOTH ONLINE OR BOTH OFFLINE
        if (online()){  //BOTH ONLINE, COMPARISON BY VIEWER COUNT
            return (channel->getViewers() >= stritem.getChannel()->getViewers());
        }
        else{ //BOTH OFFLINE, COMPARISON BY DEFAULT QSTRING METHOD
            return (QString::compare(this->text(),stritem.text()) < 0);
        }
    }
    return online();    //OTHER IS ONLINE AND OTHER IS NOT
}
