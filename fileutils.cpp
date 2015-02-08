#include "fileutils.h"

std::string util::notabs(std::string str){
	std::string newstr;
	for (size_t i=0; i<str.length(); i++){
		if (str[i] != '\t') newstr += str[i];
	}
	return newstr;
}

std::string util::readFile(const char *path){
	std::ifstream file(path);
	if (!file.is_open()){
		std::cout<<"Error: File not found.\n";
		return "";
	}
	std::string data;
	std::string str = "";
	while (getline(file, data))
		str += data;
	file.close();
	return notabs(str);
}

bool util::writeFile(const char *path, std::string data){
	std::ofstream file(path);
	if (!file.is_open()){
		std::cout<<"Error: File not found.\n";
		return 0;	
	}
	file << data;		
	file.close();
	return 1;
}
