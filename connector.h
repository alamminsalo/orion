#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "fileutils.h"
#include <curl/curl.h>
#include <iostream>
#include <string>

struct Data{
    char* str;
	size_t bytes;
};

static size_t write_data(char*,size_t,size_t,void*);

namespace conn{
	std::string Get(const std::string);
	void GetFile(const std::string,const std::string);
}
/*
class Connector{
	//CURL *curl;
	public:
	Connector();
	~Connector();

	const char* Get(const char*);
	bool GetFile(const char*,const char*);
};
*/
#endif //CONNECTOR_H
