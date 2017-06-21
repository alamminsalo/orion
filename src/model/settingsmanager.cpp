#include "settingsmanager.h"
#include "../network/httpserver.h"

SettingsManager *SettingsManager::instance = 0;

SettingsManager::SettingsManager(QObject *parent) :
    QObject(parent)
{
    settings = new QSettings("orion.application", "Orion", this);

    //Initial values
    setAlert(true);
    setCloseToTray(false);
    setAlertPosition(1);
    setMinimizeOnStartup(false);
    setTextScaleFactor(1.0);
    setVolumeLevel(100);
    setOfflineNotifications(false);
    setAccessToken("");

    //Load values from settings, notifying changes as needed
    if (settings->contains("alert")) {
        setAlert(settings->value("alert").toBool());
    }

    if (settings->contains("alertPosition")) {
        setAlertPosition(settings->value("alertPosition").toInt());
    }

    if (settings->contains("closeToTray")) {
        setCloseToTray(settings->value("closeToTray").toBool());
    }

    if (settings->contains("minimizeOnStartup")) {
        setMinimizeOnStartup(settings->value("minimizeOnStartup").toBool());
    }

    if (settings->contains("quality")) {
        setQuality(settings->value("quality").toString());
    }

    if (settings->contains("volumeLevel")) {
        setVolumeLevel(settings->value("volumeLevel").toInt());
    }

    if(settings->contains("swapChat")) {
        setSwapChat(settings->value("swapChat").toBool());
    }

    if (settings->contains("textScaleFactor")) {
        setTextScaleFactor(settings->value("textScaleFactor").toDouble());
    }

    if(settings->contains("offlineNotifications")) {
        setOfflineNotifications(settings->value("offlineNotifications").toBool());
    }

    if (settings->contains("accessToken"))
        setAccessToken(settings->value("accessToken").toString());

    //Connections
    connect(HttpServer::getInstance(), &HttpServer::codeReceived, this, &SettingsManager::setAccessToken);
}

SettingsManager *SettingsManager::getInstance()
{
    if (!instance)
        instance = new SettingsManager();
    return instance;
}

bool SettingsManager::alert() const
{
    return mAlert;
}

void SettingsManager::setAlert(bool alert)
{
    if (mAlert != alert) {
        mAlert = alert;
        settings->setValue("alert", alert);
        emit alertChanged();
    }
}

bool SettingsManager::closeToTray() const
{
    return mCloseToTray;
}

void SettingsManager::setCloseToTray(bool closeToTray)
{
    if (mCloseToTray != closeToTray) {
        mCloseToTray = closeToTray;
        settings->setValue("closeToTray", closeToTray);
        emit closeToTrayChanged();
    }
}

int SettingsManager::alertPosition() const
{
    return mAlertPosition;
}

void SettingsManager::setAlertPosition(int alertPosition)
{
    if (alertPosition != mAlertPosition) {
        mAlertPosition = alertPosition;
        emit alertPositionChanged();
        settings->setValue("alertPosition", alertPosition);
    }
}

int SettingsManager::volumeLevel() const
{
    return mVolumeLevel;
}

void SettingsManager::setVolumeLevel(int volumeLevel)
{
    if (mVolumeLevel != volumeLevel) {
        mVolumeLevel = volumeLevel;
        settings->setValue("volumeLevel", volumeLevel);
        emit volumeLevelChanged();
    }
}

bool SettingsManager::minimizeOnStartup() const
{
    return mMinimizeOnStartup;
}

void SettingsManager::setMinimizeOnStartup(bool minimizeOnStartup)
{
    if (mMinimizeOnStartup != minimizeOnStartup) {
        mMinimizeOnStartup = minimizeOnStartup;
        settings->setValue("minimizeOnStartup", minimizeOnStartup);
        emit minimizeOnStartupChanged();
    }
}

bool SettingsManager::swapChat() const
{
    return mSwapChat;
}

void SettingsManager::setSwapChat(bool swapChat)
{
    if (mSwapChat != swapChat) {
        mSwapChat = swapChat;
        settings->setValue("swapChat", swapChat);
        emit swapChatChanged();
    }
}

bool SettingsManager::offlineNotifications() const
{
    return mOfflineNotifications;
}

void SettingsManager::setOfflineNotifications(bool offlineNotifications)
{
    if (mOfflineNotifications != offlineNotifications) {
        mOfflineNotifications = offlineNotifications;
        settings->setValue("offlineNotifications", offlineNotifications);
        emit offlineNotificationsChanged();
    }
}

double SettingsManager::textScaleFactor() const
{
    return mTextScaleFactor;
}

void SettingsManager::setTextScaleFactor(double textScaleFactor)
{
    if (mTextScaleFactor != textScaleFactor) {
        mTextScaleFactor = textScaleFactor;
        settings->setValue("textScaleFactor", textScaleFactor);
        emit textScaleFactorChanged();
    }
}

QString SettingsManager::quality() const
{
    return mQuality;
}

void SettingsManager::setQuality(const QString &quality)
{
    if (mQuality != quality) {
        mQuality = quality;
        settings->setValue("quality", quality);
        emit qualityChanged();
    }
}

QString SettingsManager::accessToken() const
{
    return mAccessToken;
}

void SettingsManager::setAccessToken(const QString accessToken)
{
    if (mAccessToken != accessToken) {
        mAccessToken = accessToken;
        settings->setValue("accessToken", accessToken);
        emit accessTokenChanged(accessToken);
    }
}

bool SettingsManager::hasAccessToken() const
{
    return !mAccessToken.isEmpty();
}
