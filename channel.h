#ifndef CHANNEL_H
#define CHANNEL_H

#include <ctime>
#include <string>
#include <stdlib.h>
#include <iostream>
#include "fileutils.h"
#include <QObject>

class Channel: public QObject{

    Q_OBJECT

	private:
	std::string name;
	std::string uri;
	std::string info;
	std::string alert;
    std::string game;
    unsigned int viewers;
	bool online;
    bool changed;
	time_t timestamp;
    std::string logopath;
    std::string previewpath;
    std::string logouri;
    std::string previewuri;

	public:
        Channel();
        Channel(const char*);
        Channel(const char*, const char*, const char*);
        Channel(const char*, const char*, const char*, const char*);
        Channel(const char*, const char*, const char*, const char*, time_t);
        Channel(const char*, const char*, const char*, const char*, time_t, const char*);
        Channel(const char*, const char*, const char*, const char*, time_t, const char*, const char*);
        Channel(const Channel&);
        ~Channel();
        const std::string getJSON() const;
		void setName(const char*);	
		void setURIName(const char*);	
		void setInfo(const char*);	
        void setAlert(const char*);
        void setLastSeen(time_t);
		std::string lastOnline();
		void updateTime();
		time_t getTime();
		std::string getName();
		std::string getUriName();
		std::string getFullUri();
		std::string getInfo();
		bool hasAlert();
		void setOnline(const bool);
        bool isOnline();
        bool isEmpty();
		void clear();
        void setLogoPath(const char*);
        std::string getLogoPath();
        void setPreviewPath(const char*);
        std::string getPreviewPath();
        bool isChanged();
        void setChanged(bool b);

        std::string getLogourl();
        void setLogourl(const char*);

        std::string getPreviewurl();
        void setPreviewurl(const char*);

        unsigned int getViewers() const;
        void setViewers(unsigned int value);

        std::string getGame() const;
        void setGame(const std::string &value);

signals:
        void updated();
        void iconUpdated();
};

#endif //CHANNEL_H
