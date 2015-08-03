#include "streamitem.h"
#include <QTimer>
#include <QApplication>

StreamItem::StreamItem(Channel* channel)
{
    this->channel = channel;
    update();
    updateIcon();
}

StreamItem::~StreamItem(){
    qDebug() << "Destroyer: StreamItem";
}


void StreamItem::update(){
        QString title = getName();
        QString image = "<img src=\"";
        image += channel->getPreviewPath();
        image += "\"></img>";
        QString tooltip = "<table><tr><td><b>Playing: "+getGame()+"</b></td></tr><tr><td>"+getInfo()+"</td></tr>";
        tooltip += "<tr><td>"+image+"</td></tr>";

        if (!online()){
            tooltip += "<tr><td>Last seen: ";
            tooltip += channel->lastOnline();
            tooltip += "</td></tr>";
            setTextColor(QColor(200, 200, 200));
        }
        else {
            QString viewercount = QString::number(channel->getViewers());
            tooltip += "<tr><td><b>Now streaming:</b> <i>"+viewercount+" viewers</i></td></tr>";
            setTextColor(QColor(0, 0, 0));
        }

        this->setText(title);

        tooltip += "</table>";
        QListWidgetItem::setToolTip(tooltip);
}

void StreamItem::updateIcon()
{
    if (!channel->getLogoPath().isEmpty() && QFile::exists(channel->getLogoPath())){
        setIcon(QIcon(channel->getLogoPath()));
    }
    else
        setIcon(QIcon("resources/logos/default.png"));
}

Channel* StreamItem::getChannel() const{
    return channel;
}

bool StreamItem::online() const{
    return channel->isOnline();
}

const QString StreamItem::getName(){
    if (!channel->getName().isEmpty())
        return channel->getName();
    return channel->getUriName();
}

const QString StreamItem::getUriName(){
    return channel->getUriName();
}

const QString StreamItem::getInfo(){
    return channel->getInfo();
}

const QString StreamItem::getGame()
{
    return channel->getGame();
}

bool StreamItem::operator< (const QListWidgetItem& other) const{
    const StreamItem& stritem = dynamic_cast<const StreamItem&>(other);
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
