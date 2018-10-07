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
    mMultipleInstances = false;
    mAlertPosition = 1;
    mMinimizeOnStartup = false;
    mTextScaleFactor = 1.0;
    mVolumeLevel = 100;
    mOfflineNotifications = false;
    mAccessToken = "";
    mQuality = "source";
    mChatEdge = 1;
    mLightTheme = false;
    mFont = "";
    mKeepOnTop = false;

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

    if (settings->contains("multipleInstances")) {
        setMultipleInstances(settings->value("multipleInstances").toBool());
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

    if(settings->contains("chatEdge")) {
        setChatEdge(settings->value("chatEdge").toInt());
    }

    if (settings->contains("textScaleFactor")) {
        setTextScaleFactor(settings->value("textScaleFactor").toDouble());
    }

    if(settings->contains("offlineNotifications")) {
        setOfflineNotifications(settings->value("offlineNotifications").toBool());
    }

    if (settings->contains("lightTheme")) {
        setLightTheme(settings->value("lightTheme").toBool());
    }

    if (settings->contains("accessToken")) {
        setAccessToken(settings->value("accessToken").toString());
    }

    if (settings->contains("font")) {
        setFont(settings->value("font").toString());
    }

    if (settings->contains("keepOnTop")) {
        setKeepOnTop(settings->value("keepOnTop").toBool());
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

bool SettingsManager::multipleInstances() const
{
    return mMultipleInstances;
}

void SettingsManager::setMultipleInstances(bool multipleInstances)
{
    if (mMultipleInstances != multipleInstances) {
        mMultipleInstances = multipleInstances;
        settings->setValue("multipleInstances", multipleInstances);

        qDebug() << "multipleInstances changed to" << multipleInstances;
    }
    emit multipleInstancesChanged();
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
#ifdef Q_OS_ANDROID
    return false;
#else
    return mMinimizeOnStartup;
#endif
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

int SettingsManager::chatEdge() const
{
#ifdef Q_OS_ANDROID
    return 2; // bottom
#else
    return mChatEdge;
#endif
}

void SettingsManager::setChatEdge(int chatEdge)
{
    if (mChatEdge != chatEdge) {
        mChatEdge = chatEdge;
        settings->setValue("chatEdge", chatEdge);

        qDebug() << "chatEdge changed to" << chatEdge;
    }
    emit chatEdgeChanged();
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

bool SettingsManager::lightTheme() const
{
    return mLightTheme;
}

void SettingsManager::setLightTheme(bool lightTheme)
{
    if (mLightTheme != lightTheme) {
        mLightTheme = lightTheme;
        settings->setValue("lightTheme", lightTheme);
        qDebug() << "theme changed!";
    }
    emit lightThemeChanged();
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

#include <QVersionNumber>
bool SettingsManager::isNewerVersion(QString version) const
{
    return QVersionNumber::fromString(version.replace('v',"")) > QVersionNumber::fromString(appVersion().replace('v', ""));
}

QString SettingsManager::font() const
{
    return mFont;
}

void SettingsManager::setFont(const QString &font)
{
    if (mFont != font) {
        mFont = font;
        settings->setValue("font", font);
    }
    emit fontChanged();
}

bool SettingsManager::versionCheckEnabled()
{
#ifdef VERSION_CHECK_ENABLED
    return true;
#else
    return false;
#endif
}

bool SettingsManager::keepOnTop() const
{
    return mKeepOnTop;
}

void SettingsManager::setKeepOnTop(bool keepOnTop)
{
    if (mKeepOnTop != keepOnTop) {
        mKeepOnTop = keepOnTop;
        settings->setValue("keepOnTop", keepOnTop);
    }
    emit keepOnTopChanged();
}
