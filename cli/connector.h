#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "fileutils.h"
#include <curl/curl.h>
#include <iostream>
#include <string>

namespace conn{
    size_t write_data(char*,size_t,size_t,void*);
	std::string Get(const std::string);
	void GetFile(const std::string,const std::string);
}

#endif //CONNECTOR_H
