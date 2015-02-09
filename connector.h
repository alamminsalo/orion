#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "fileutils.h"
#include <curl/curl.h>
#include <iostream>
#include <string>

struct Data{
	char *str;
	size_t bytes;
};

size_t write_data(void*,size_t,size_t,Data*);

class Connector{
	CURL *curl;

	public:
	Connector();
	~Connector();

	const char* Get(const char*);
	bool GetFile(const char*,const char*);
};

#endif //CONNECTOR_H
