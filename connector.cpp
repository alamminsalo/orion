#include "connector.h"

Connector::Connector(){
	curl_global_init(CURL_GLOBAL_DEFAULT);
	this->curl = curl_easy_init();
}

Connector::~Connector(){
	curl_easy_cleanup(curl);
}

size_t write_data(void *ptr, size_t size, size_t nmemb, respData *data){
	size_t numBytes = size * nmemb;

	if (data->str)
		*(data->str) += std::string((char*)ptr,numBytes);

	return numBytes;
}


std::string Connector::Get(const char *uri){
	std::string json;	
	if (curl){
		CURLcode res;
		struct curl_slist *chunk = NULL;

		chunk = curl_slist_append(chunk,"Accept:application/json");

		curl_easy_setopt(curl,CURLOPT_HTTPHEADER, chunk);
		curl_easy_setopt(curl,CURLOPT_URL,uri);

		respData data;
		data.str = &json;

		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, &write_data);
		curl_easy_setopt(curl,CURLOPT_WRITEDATA,&data);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK){
			std::cout<<"curl_easy_perform() failed.\n";
			json = "";
		}
	}		

	return json;
}
