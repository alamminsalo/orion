#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include "channel.h"
#include "game.h"
#include "../util/fileutils.h"
#include "../network/networkmanager.h"
#include <vector>
#include <QStringRef>
#include <QObject>
#include <QDir>
#include <QProcess>

//#include <qqmllist.h>

#define TWITCH_URI          "https://api.twitch.tv/kraken"
#define DATAURI             "./data.json"
#define DEFAULT_LOGO_URL    "http://static-cdn.jtvnw.net/jtv_user_pictures/xarth/404_user_150x150.png"
#define DIALOG_FILE         "resources/scripts/dialog.sh"
#define PLAY_FILE           "resources/scripts/play.sh"

class NetworkManager;

class ChannelManager: public QObject{
    Q_OBJECT
    Q_PROPERTY(QVariantList channels READ getChannelsList NOTIFY channelsUpdated)
    Q_PROPERTY(QVariantList games READ getGamesList NOTIFY gamesUpdated)

    protected:
        QList<Channel*> channels;
        QList<Game*> games;
        NetworkManager* netman;
        bool channelsChanged;

	
	public:
		ChannelManager();

		~ChannelManager();	
        void load();
        void load(const QString&);
        void save();
        bool readJSON(const QString&);
        bool writeJSON(const QString&);

        void checkChannels();
        void add(Channel *channel);
        void add(const char*,const char*,const char*,const char*);
		void remove(const char*);
        void remove(Channel*);
        void add(const QString&);
        Channel *find(const QString&);
        int findPos(const QString&);
        QList<Channel*>& getChannels(){ return channels; }
		void clearData();
        void play(Channel*);
        void checkResources();

        void updateChannel(Channel*);
        void updateChannels(const QList<Channel*>&);
        void updateStreams(const QList<Channel*>&);
        void updateStream(Channel*);
        void updateGames(const QList<Game*>&);

        QVariantList getChannelsList();
        QVariantList getGamesList();

    signals:
        void channelExists(Channel*);
        void channelNotFound(Channel*);
        void channelStateChanged(Channel*);
        void newChannel(Channel*);
        void gamesUpdated();
        void channelsUpdated();

    public slots:
        void checkStreams();
        void getGames();
        void notify(Channel*);

};

#endif //CHANNEL_MANAGER_H
