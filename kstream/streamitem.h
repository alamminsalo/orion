#ifndef STREAMITEM_H
#define STREAMITEM_H

#include <QListWidgetItem>
#include <QDebug>
#include "../channel.h"

class Channel;

class StreamItem: public QListWidgetItem
{
protected:
    Channel* channel;

public:
    explicit StreamItem(QWidget *parent = 0);
    StreamItem(Channel*);
    ~StreamItem();

    bool online() const;

    const QString getName();

    const QString getUriName();

    const QString getInfo();

    virtual bool operator< (const QListWidgetItem& other) const;

    void update();

    Channel* getChannel();

};

#endif // STREAMITEM_H
