#ifndef CHANNEL_H
#define CHANNEL_H

#include <ctime>
#include <string>
#include <stdlib.h>

class Channel{
	private:
	std::string name;
	std::string uri;
	std::string info;
	std::string alert;
	bool online;
	time_t timestamp;
    std::string logopath;
    std::string previewpath;

	public:
		Channel();
        Channel(const char*);
        Channel(const char*, const char*, const char*);
        Channel(const char*, const char*, const char*, const char*);
        Channel(const char*, const char*, const char*, const char*, time_t);
        Channel(const char*, const char*, const char*, const char*, time_t, const char*);
        Channel(const char*, const char*, const char*, const char*, time_t, const char*, const char*);
        Channel(const Channel&);
		~Channel(){};
		std::string getJSON();
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
};

#endif //CHANNEL_H
