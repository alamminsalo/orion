#include "settingsmanager.h"
#include "../network/httpserver.h"

SettingsManager *SettingsManager::instance = 0;

SettingsManager::SettingsManager(QObject *parent) :
    QObject(parent),
    mHiDpi(false)
{
    settings = new QSettings("orion.application", "Orion", this);

    //Initial values
    mAlert = true;
    mCloseToTray = false;
    mAlertPosition = 1;
    mMinimizeOnStartup = false;
    mTextScaleFactor = 1.0;
    mVolumeLevel = 100;
    mOfflineNotifications = false;
    mAccessToken = "";
    mQuality = "source";

    //Connections
    connect(HttpServer::getInstance(), &HttpServer::codeReceived, this, &SettingsManager::setAccessToken);
}

SettingsManager *SettingsManager::getInstance()
{
    if (!instance)
        instance = new SettingsManager();
    return instance;
}

void SettingsManager::load()
{
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

    if (settings->contains("accessToken")) {
        setAccessToken(settings->value("accessToken").toString());
    }
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

        qDebug() << "alert changed to" << alert;
    }
    emit alertChanged();
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

        qDebug() << "closeToTray changed to" << closeToTray;
    }
    emit closeToTrayChanged();
}

int SettingsManager::alertPosition() const
{
    return mAlertPosition;
}

void SettingsManager::setAlertPosition(int alertPosition)
{
    if (alertPosition != mAlertPosition) {
        mAlertPosition = alertPosition;
        settings->setValue("alertPosition", alertPosition);

        qDebug() << "alertPosition changed to" << alertPosition;
    }
    emit alertPositionChanged();
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

        qDebug() << "volumeLevel changed to" << volumeLevel;
    }
    emit volumeLevelChanged();
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

        qDebug() << "minimizeOnStartup changed to" << minimizeOnStartup;
    }
    emit minimizeOnStartupChanged();
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

        qDebug() << "swapChat changed to" << swapChat;
    }
    emit swapChatChanged();
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

        qDebug() << "offlineNotifications changed to" << offlineNotifications;
    }
    emit offlineNotificationsChanged();
}

double SettingsManager::textScaleFactor() const
{
    return mTextScaleFactor;
}

void SettingsManager::setTextScaleFactor(double textScaleFactor)
{
    //Validate min/max
    if (textScaleFactor < 0.5 || textScaleFactor > 3.0)
        return;

    if (mTextScaleFactor != textScaleFactor) {
        mTextScaleFactor = textScaleFactor;
        settings->setValue("textScaleFactor", textScaleFactor);

        qDebug() << "textScaleFactor changed to" << textScaleFactor;
    }
    emit textScaleFactorChanged();
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

        qDebug() << "quality changed to" << quality;
    }
    emit qualityChanged();
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

        qDebug() << "accessToken changed!";
    }
    emit accessTokenChanged(accessToken);
}

bool SettingsManager::hasAccessToken() const
{
    return !mAccessToken.isEmpty();
}

bool SettingsManager::hiDpi() const {
    return mHiDpi;
}

void SettingsManager::setHiDpi(bool dpi)
{
    mHiDpi = dpi;
    qDebug() << "hiDpi" << mHiDpi;
}

QString SettingsManager::appName() const
{
    return APP_NAME;
}

QString SettingsManager::appVersion() const
{
    return APP_VERSION;
}

QString SettingsManager::appPlayerBackend() const
{
    return PLAYER_BACKEND;
}
