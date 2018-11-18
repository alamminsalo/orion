#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include "singletonprovider.h"

class SettingsManager : public QObject
{
    QML_SINGLETON
    Q_OBJECT

    Q_PROPERTY(bool alert READ alert WRITE setAlert NOTIFY alertChanged)
    Q_PROPERTY(bool multipleInstances READ multipleInstances WRITE setMultipleInstances NOTIFY multipleInstancesChanged)
    Q_PROPERTY(int alertPosition READ alertPosition WRITE setAlertPosition NOTIFY alertPositionChanged)
    Q_PROPERTY(int volumeLevel READ volumeLevel WRITE setVolumeLevel NOTIFY volumeLevelChanged)
    Q_PROPERTY(bool minimizeOnStartup READ minimizeOnStartup WRITE setMinimizeOnStartup NOTIFY minimizeOnStartupChanged)
    Q_PROPERTY(int chatEdge READ chatEdge WRITE setChatEdge NOTIFY chatEdgeChanged)
    Q_PROPERTY(bool offlineNotifications READ offlineNotifications WRITE setOfflineNotifications NOTIFY offlineNotificationsChanged)
    Q_PROPERTY(double textScaleFactor READ textScaleFactor WRITE setTextScaleFactor NOTIFY textScaleFactorChanged)
    Q_PROPERTY(QString opengl READ opengl WRITE setOpengl NOTIFY openglChanged)
    Q_PROPERTY(QString quality READ quality WRITE setQuality NOTIFY qualityChanged)
    Q_PROPERTY(QString decoder READ decoder WRITE setDecoder NOTIFY decoderChanged)
    Q_PROPERTY(QString backend READ backend WRITE setBackend NOTIFY backendChanged)
    Q_PROPERTY(QStringList backends READ backends NOTIFY backendsChanged)
    Q_PROPERTY(QString accessToken READ accessToken WRITE setAccessToken NOTIFY accessTokenChanged)
    Q_PROPERTY(bool hasAccessToken READ hasAccessToken NOTIFY accessTokenChanged)
    Q_PROPERTY(bool lightTheme READ lightTheme WRITE setLightTheme NOTIFY lightThemeChanged)
    Q_PROPERTY(bool pastelColors READ pastelColors WRITE setPastelColors NOTIFY pastelColorsChanged)
    Q_PROPERTY(bool clickTogglePause READ clickTogglePause WRITE setClickTogglePause NOTIFY clickTogglePauseChanged)
    Q_PROPERTY(bool autoScrollSmoothing READ autoScrollSmoothing WRITE setAutoScrollSmoothing NOTIFY autoScrollSmoothingChanged)
    Q_PROPERTY(QString font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(bool versionCheckEnabled READ versionCheckEnabled)
    Q_PROPERTY(bool keepOnTop READ keepOnTop WRITE setKeepOnTop NOTIFY keepOnTopChanged)

    bool mAlert = true;
    bool mMultipleInstances = false;
    int mAlertPosition = 1;
    int mVolumeLevel = 100;
    bool mMinimizeOnStartup = false;
    bool mSwapChat = false;
    bool mOfflineNotifications = false;
    double mTextScaleFactor = 1.0;
#ifdef Q_OS_WIN
    QString mOpengl = "angle (d3d9)";
#else
    QString mOpengl = "opengl es";
#endif
    QString mQuality = "source";
    QString mDecoder = "auto";

#ifdef MPV_PLAYER
    QString mBackend = "mpv";
#elif defined(QTAV_PLAYER)
    QString mBackend = "qtav";
#elif defined(MULTIMEDIA_PLAYER)
    QString mBackend = "multimedia";
#else
    #error unknown backend
#endif
    QStringList mBackends = {
    #ifdef MPV_PLAYER
        "mpv",
    #endif
    #ifdef QTAV_PLAYER
        "qtav",
    #endif
    #ifdef MULTIMEDIA_PLAYER
        "multimedia",
    #endif
    };
    QString mAccessToken = "";
    int mChatEdge = 1;
    bool mLightTheme = false;
    bool mPastelColors = true;
    bool mClickTogglePause = true;
    bool mAutoScrollSmoothing = true;
    QString mFont = "";

    bool mHiDpi = false;
    bool mKeepOnTop = false;

    explicit SettingsManager(QObject *parent = nullptr);
public:
    static SettingsManager *getInstance();

    bool alert() const;
    void setAlert(bool alert);

    bool multipleInstances() const;
    void setMultipleInstances(bool multipleInstances);

    int alertPosition() const;
    void setAlertPosition(int alertPosition);

    int volumeLevel() const;
    void setVolumeLevel(int volumeLevel);

    bool minimizeOnStartup() const;
    void setMinimizeOnStartup(bool minimizeOnStartup);

    int chatEdge() const;
    void setChatEdge(int chatEdge);

    bool offlineNotifications() const;
    void setOfflineNotifications(bool offlineNotifications);

    double textScaleFactor() const;
    void setTextScaleFactor(double textScaleFactor);

    QString opengl() const;
    void setOpengl(const QString &opengl);

    QString quality() const;
    void setQuality(const QString &quality);

    QString decoder() const;
    void setDecoder(const QString &decoder);

    QString accessToken() const;

    void setHiDpi(bool dpi);

    bool lightTheme() const;
    void setLightTheme(bool lightTheme);

    QString font() const;
    void setFont(const QString &font);

    bool versionCheckEnabled();

    bool keepOnTop() const;
    void setKeepOnTop(bool keepOnTop);

    QString backend() const;
    void setBackend(const QString &backend);

    QStringList backends() const;

    bool pastelColors() const;
    void setPastelColors(bool pastelColors);

    bool clickTogglePause() const;
    void setClickTogglePause(bool clickTogglePause);

    bool autoScrollSmoothing() const;
    void setAutoScrollSmoothing(bool autoScrollSmoothing);

signals:
    void alertChanged();
    void multipleInstancesChanged();
    void alertPositionChanged();
    void volumeLevelChanged();
    void minimizeOnStartupChanged();
    void chatEdgeChanged();
    void offlineNotificationsChanged();
    void textScaleFactorChanged();
    void openglChanged();
    void qualityChanged();
    void decoderChanged();
    void backendChanged();
    void backendsChanged();
    void lightThemeChanged();
    void accessTokenChanged(QString accessToken);
    void fontChanged();
    void keepOnTopChanged();
    void pastelColorsChanged();
    void clickTogglePauseChanged();
    void autoScrollSmoothingChanged();

public slots:
    void setAccessToken(const QString accessToken);
    bool hasAccessToken() const;
    void load();

    bool hiDpi() const;
    bool isNewerVersion(QString version) const;

private:
    QSettings settings;
};

#endif // SETTINGSMANAGER_H
