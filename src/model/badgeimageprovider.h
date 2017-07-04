#pragma once

#include "imageprovider.h"

class ChannelManager;

class BadgeImageProvider : public ImageProvider {
    Q_OBJECT
public:
    BadgeImageProvider();
    void setChannelName(QString channelName) { _channelName = channelName; }
    void setChannelId(QString channelId) { _channelId = channelId; }
    virtual QString getCanonicalKey(QString key);
protected:
    virtual const QUrl getUrlForKey(QString & key);
private:
    QString _channelName;
    QString _channelId;
};

class BitsImageProvider : public ImageProvider {
    Q_OBJECT

public:
    BitsImageProvider();
    void setChannelId(int channelId) { _channelId = channelId; }
    virtual QString getCanonicalKey(QString key);
protected:
    virtual const QUrl getUrlForKey(QString & key);
private:
    int _channelId;
};
