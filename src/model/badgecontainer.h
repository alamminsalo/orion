#pragma once

#include <QObject>
#include "../network/networkmanager.h"
#include "imageprovider.h"
#include "badgeimageprovider.h"

#include <QQmlEngine>
#include <QJSEngine>

class BadgeContainer: public QObject
{
    Q_OBJECT

    bool haveEmoteSets;
    QList<int> lastRequestedEmoteSetIDs;
    QMap<int, QMap<int, QString>> lastEmoteSets;
    QMap<QString, QMap<QString, QMap<QString, QString>>> channelBadgeUrls;
    QMap<QString, QMap<QString, QMap<QString, QMap<QString, QString>>>> channelBadgeBetaUrls;
    QMap<QString, QMap<QString, QString>> channelBttvEmotes;
    QMap<int, QMap<QString, QMap<QString, QString>>> channelBitsUrls;
    QMap<int, QMap<QString, QMap<QString, QString>>> channelBitsColors;

    NetworkManager *netman;

    static BadgeContainer *instance;

    BadgeImageProvider badgeImageProvider;
    BitsImageProvider bitsImageProvider;

    BadgeContainer();
public:
    static BadgeContainer *getInstance();

    static QObject *provider(QQmlEngine */*eng*/, QJSEngine */*jseng*/) {
        QQmlEngine::setObjectOwnership(getInstance(), QQmlEngine::CppOwnership);
        return getInstance();
    }

    BadgeImageProvider * getBadgeImageProvider() {
        return &badgeImageProvider;
    }

    BitsImageProvider * getBitsImageProvider() {
        return &bitsImageProvider;
    }

    bool getChannelBadgeUrl(const QString channelId, const QString badgeName, const QString imageFormat, QString & outUrl) const;

    bool getChannelBadgeBetaUrl(const QString channel, const QString badgeName, const QString version, const QString imageFormat, QString & outUrl) const;

    bool getChannelBitsUrl(const int channelId, const QString & prefix, const QString & minBits, QString & outUrl) const;

    const QUrl getBitsUrlForKey(const QString & key) const;

    bool getChannelBitsColor(const int channelId, const QString & prefix, const QString & minBits, QString & outColor);

public slots:
    void onEmoteSetsUpdated(const QMap<int, QMap<int, QString>>);
    void innerChannelBadgeUrlsLoaded(const quint64, const QMap<QString, QMap<QString, QString>> badgeUrls);
    void innerChannelBadgeBetaUrlsLoaded(const int channelId, const QMap<QString, QMap<QString, QMap<QString, QString>>> badgeData);
    void innerGlobalBadgeBetaUrlsLoaded(const QMap<QString, QMap<QString, QMap<QString, QString>>> badgeData);
    void innerChannelBitsDataLoaded(int channelID, BitsQStringsMap channelBitsUrls, BitsQStringsMap channelBitsColors);
    void innerGlobalBitsDataLoaded(BitsQStringsMap globalBitsUrls, BitsQStringsMap globalBitsColors);
    void innerChannelBttvEmotesLoaded(const QString channel, QMap<QString, QString> & emotesByCode);
    void innerGlobalBttvEmotesLoaded(QMap<QString, QString> & emotesByCode);
    bool loadEmoteSets(bool reload, const QList<int> &emoteSetIDs);
    bool loadChannelBadgeUrls(const quint64 channelId);
    bool loadChannelBetaBadgeUrls(int channel);
    bool loadChannelBitsUrls(int channel);
    bool loadChannelBttvEmotes(const QString channel);

signals:
    void emoteSetsLoaded(QVariantMap emoteSets);
    void channelBadgeUrlsLoaded(const quint64 channelId, QVariantMap badgeUrls);
    void channelBadgeBetaUrlsLoaded(const QString &channel, QVariantMap badgeSetData);

    void channelBitsUrlsLoaded(const int channelID, BitsQStringsMap bitsUrls, BitsQStringsMap bitsColors);

    void channelBttvEmotesLoaded(const QString channel, QMap<QString, QString> emotesByCode);
};
