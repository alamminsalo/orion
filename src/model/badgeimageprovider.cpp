#include "badgeimageprovider.h"
#include "../global.h"
#include "badgecontainer.h"

BadgeImageProvider::BadgeImageProvider() : ImageProvider("badge", ".png") {

}

QString BadgeImageProvider::getCanonicalKey(QString key) {
    /** Resolve a key with just a badge name and version, specific to the current room, to a globally unique key for an official API or beta API badge */
    QString url;

    const QString betaImageFormat = global::hiDpi ? "image_url_2x" : "image_url_1x";
    const QString officialImageFormat = "image";

    int splitPos = key.indexOf("-");
    if (splitPos != -1) {
        const QString badge = key.left(splitPos);
        const QString version = key.mid(splitPos + 1);
        //qDebug() << "badge hunt: channel name" << _channelName << "channel id" << _channelId << "badge" << badge << "version" << version;

        if (BadgeContainer::getInstance()->getChannelBadgeBetaUrl(_channelId, badge, version, betaImageFormat, url)) {
            return QList<QString>({ _channelId, badge, version, betaImageFormat }).join("-");
        }
        if (BadgeContainer::getInstance()->getChannelBadgeBetaUrl("GLOBAL", badge, version, betaImageFormat, url)) {
            return QList<QString>({ "GLOBAL", badge, version, betaImageFormat }).join("-");
        }
        if (BadgeContainer::getInstance()->getChannelBadgeUrl(_channelId, badge, officialImageFormat, url)) {
            return QList<QString>({ _channelName, badge, officialImageFormat }).join("-");
        }
        if (BadgeContainer::getInstance()->getChannelBadgeUrl("GLOBAL", badge, officialImageFormat, url)) {
            return QList<QString>({ "GLOBAL", badge, officialImageFormat }).join("-");
        }
    }

    qDebug() << "getCanonicalKey for badge" << key << "could not find a badge";
    return key;
}

const QUrl BadgeImageProvider::getUrlForKey(QString & key) {
    QString url;

    QList<QString> parts = key.split("-");
    if (parts.length() == 3) {
        if (BadgeContainer::getInstance()->getChannelBadgeUrl(parts[0], parts[1], parts[2], url)) {
            return url;
        }
    }
    else if (parts.length() == 4) {
        if (BadgeContainer::getInstance()->getChannelBadgeBetaUrl(parts[0], parts[1], parts[2], parts[3], url)) {
            return url;
        }
    }
    qDebug() << "Invalid badge cache key" << key;
    return QUrl();
}

BitsImageProvider::BitsImageProvider() : ImageProvider("bits", ".gif") {

}

QString BitsImageProvider::getCanonicalKey(QString key) {
    // input key has a prefix and a bits level, separated by a -

    QString globalUrl;
    QString channelUrl;

    const QString theme = "dark";
    const QString type = "animated";
    const QString size = global::hiDpi ? "2" : "1";

    int splitPos = key.indexOf('-');
    if (splitPos != -1) {
        QString prefix = key.left(splitPos);
        QString minBits = key.mid(splitPos + 1);

        bool foundGlobalUrl = BadgeContainer::getInstance()->getChannelBitsUrl(-1, prefix, minBits, globalUrl);

        if (BadgeContainer::getInstance()->getChannelBitsUrl(_channelId, prefix, minBits, channelUrl)) {
            if (!foundGlobalUrl || channelUrl != globalUrl) {
                return QList<QString>({ QString::number(_channelId), theme, type, size, prefix, minBits }).join("-");
            }
        }
        if (foundGlobalUrl) {
            return QList<QString>({ "GLOBAL", theme, type, size, prefix, minBits }).join("-");
        }
    }
    qDebug() << "can't canonicalize" << key << "couldn't find that bits badge";
    return key;
}

const QUrl BitsImageProvider::getUrlForKey(QString & key) {
    return BadgeContainer::getInstance()->getBitsUrlForKey(key);
}
