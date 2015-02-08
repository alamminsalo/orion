#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <curl/curl.h>
#include <iostream>
#include <string>

struct respData{
	std::string *str;
};
size_t write_data(void*,size_t,size_t,respData*);

class Connector{
	CURL *curl;

	public:
	Connector();
	~Connector();

	std::string Get(const char*);
};

#endif //CONNECTOR_H
