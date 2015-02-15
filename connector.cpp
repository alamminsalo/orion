#include "connector.h"
/*
Connector::Connector(){ 
//	curl_global_init(CURL_GLOBAL_DEFAULT); 
	//curl = curl_easy_init();
}

Connector::~Connector(){
}
*/

size_t conn::write_data(char *ptr, size_t size, size_t nmemb, void *data){
	size_t numBytes = size * nmemb;

    if (data) {
        static_cast<std::string*>(data)->append(ptr,numBytes);
      }

	return numBytes;
}


std::string conn::Get(const std::string uri){
	std::string data;
	CURL* curl = curl_easy_init();
	if (curl){
		CURLcode res;

		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl,CURLOPT_WRITEDATA,&data);
		curl_easy_setopt(curl,CURLOPT_URL,uri.c_str());

		res = curl_easy_perform(curl);
		if (res != CURLE_OK){
			std::cout<<"curl_easy_perform() failed with code "<<res<<"\n";
            data.clear();
		}
	}		
	curl_easy_cleanup(curl);
    //if (data.str)
     //   *(data.str+data.bytes) = '\0'; //Clear trailing characters

    return data;
}

void conn::GetFile(const std::string uri, const std::string path){
	CURL* curl = curl_easy_init();
	if (curl){
		FILE *fp = fopen(path.c_str(),"w");
		CURLcode res;

		struct curl_slist *chunk = NULL;

		chunk = curl_slist_append(chunk,"Accept:application/json");

		curl_easy_setopt(curl,CURLOPT_HTTPHEADER, chunk);	

		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, 0);
		curl_easy_setopt(curl,CURLOPT_WRITEDATA,fp);
		curl_easy_setopt(curl,CURLOPT_URL,uri.c_str());

		res = curl_easy_perform(curl);
		fclose(fp);
		if (res != CURLE_OK){
			std::cout<<"curl_easy_perform() failed with code "<<res<<"\n";
		}
	}		
	curl_easy_cleanup(curl);
}
