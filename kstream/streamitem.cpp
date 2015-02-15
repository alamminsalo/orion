#include "streamitem.h"

StreamItem::StreamItem(Channel* channel)
{
    this->channel = channel;
}
StreamItem::~StreamItem(){
    qDebug() << "Destroyer: StreamItem";
}

/*
 * Sets visual content for this item
 * (if content not present)
 */

void StreamItem::update(){
    if (this->icon().pixmap(QSize(32,32)).isNull()){ //MISSING ICON
        //QString logopath = "logos/" + getUriName();
        setIcon(QIcon(channel->getLogoPath().c_str()));
    }
    if (this->text().isEmpty() && !getName().isEmpty()){
        this->setText(getName()/* + "\n" + getInfo()*/);
    }
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
    else tooltip+= "<tr><td><b>Now streaming</b></td></tr>";

    tooltip += "</table>";
    setToolTip(tooltip);

    if (this->online()){
        this->setTextColor(QColor(0, 0, 0));
    }
    else this->setTextColor(QColor(200, 200, 200));
}

Channel* StreamItem::getChannel(){
    return channel;
}

bool StreamItem::online() const{
    return this->channel->isOnline();
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
    return (this->online() != stritem.online()) ? this->online() : (QString::compare(this->text(),stritem.text()) < 0);
}
