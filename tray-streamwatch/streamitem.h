#ifndef STREAMITEM_H
#define STREAMITEM_H

#include <QListWidgetItem>
#include <QDebug>
#include "../channel.h"

class StreamItem: public QListWidgetItem
{
protected:
    Channel* channel;

public:
    StreamItem(Channel*);

    bool online() const{
        return this->channel->isOnline();
    };

    const QString getName() {
        return channel->getName().c_str();
    };

    const QString getUriName(){
        return channel->getUriName().c_str();
    };

    const QString getInfo(){
        return channel->getInfo().c_str();
    };

    virtual bool operator< (const StreamItem& other) const{
        return (this->online() != other.online()) ? this->online() : (QString::compare(this->text(),other.text()) < 0);
    };

    void checkContent();

    Channel* getChannel();

};

/*
inline bool operator< (const StreamItem& left, const StreamItem& right){
    return (left.online != right.online) ? left.online : (QListWidgetItem)left < (QListWidgetItem)right;
}
*/

#endif // STREAMITEM_H
