#include "streamitem.h"

StreamItem::StreamItem(Channel* channel)
{
    this->channel = channel;
}

/*
 * Sets visual content for this item
 * (if content not present)
 */

void StreamItem::checkContent(){
    if (this->icon().pixmap(QSize(32,32)).isNull()){ //MISSING ICON
        QString logopath = "logos/" + getUriName();
        setIcon(QIcon(logopath));
    }
    if (this->text().isEmpty() && !getName().isEmpty()){
        this->setText(getName()/* + "\n" + getInfo()*/);
    }
    QString image = "<img src=\"preview/"+(this->online() ? this->getUriName() : "offline.png")+"\"></img>";
    QString tooltip = "<table><tr><td>"+getInfo()+"</td></tr>";
    tooltip += "<tr><td>"+image+"</td></tr>";
    tooltip += "</table>";


//    image += getInfo();
    setToolTip(tooltip);
}

Channel* StreamItem::getChannel(){
    return channel;
}
