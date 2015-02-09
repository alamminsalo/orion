#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <stdlib.h>
#include <fstream>
#include <iostream>

namespace util{
	std::string notabs(std::string);
	std::string readFile(const char*);
	bool writeFile(const char*,std::string);
	void writeImage(const char*,FILE*);
};

#endif
