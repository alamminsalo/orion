#ifndef CHANNEL_H
#define CHANNEL_H

#include <ctime>
#include <string>
#include <stdlib.h>

class Channel{
	private:
	std::string name;
	std::string status;
	std::string uri;
	std::string info;
	std::string alert;
	bool online;
	time_t timestamp;

	public:
		Channel();
		Channel(const char *, const char *, const char *);
		Channel(const char *, const char *, const char *, const char *);
		Channel(const char *newName, const char *newUri, const char *newInfo, const char *newAlert, time_t newTime);
		Channel(const Channel&);
		~Channel(){};

		void setName(const char *newName){
			name = newName;
		};

		void setURIName(const char *newUri){
			uri = newUri;
		};

		void setInfo(const char *newInfo){
			info = newInfo;
		};

		void enableAlert(){
			alert = "on";
		};
		
		void disableAlert(){
			alert = "off";
		};
		std::string lastOnline();

		void updateTime(){
			timestamp = time(0);
		};

		time_t getTime(){
			return timestamp;
		}

		std::string getName(){
			return name;
		};

		std::string getUriName(){
			return uri;
		};
		std::string getFullUri(){
			return "https://twitch.tv/" + uri;
		};

		std::string getInfo(){
			return info;
		};

		bool hasAlert(){
			return (alert == "on" ? true : false);
		};

		std::string getJSON();

		void setOnline(bool b){
			online = b;
		};

		bool isOnline(){
			return online;
		};
};

#endif //CHANNEL_H
