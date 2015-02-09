#include "connector.h"

Connector::Connector(){ 
	curl_global_init(CURL_GLOBAL_DEFAULT); 
	this->curl = curl_easy_init();
}

Connector::~Connector(){
	curl_easy_cleanup(curl);
}

size_t write_data(void *ptr, size_t size, size_t nmemb, Data *data){
	size_t numBytes = size * nmemb;

	if (data){
		(data->str) = (char*)ptr;
		data->bytes = numBytes;
	}

	return numBytes;
}


const char* Connector::Get(const char *uri){
	Data data;

	if (curl){
		CURLcode res;
		struct curl_slist *chunk = NULL;

		chunk = curl_slist_append(chunk,"Accept:application/json");

		curl_easy_setopt(curl,CURLOPT_HTTPHEADER, chunk);
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, &write_data);
		curl_easy_setopt(curl,CURLOPT_WRITEDATA,&data);
		curl_easy_setopt(curl,CURLOPT_URL,uri);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK){
			std::cout<<"curl_easy_perform() failed with code "<<res<<"\n";
			data.str = 0;
		}
	}		
	if (data.str)
		*(data.str+data.bytes) = '\0'; //Clear trailing characters

	return data.str;
}

bool Connector::GetFile(const char *uri, const char *file){
	FILE *fp = fopen(file,"w");

	std::cout << "Attempting to get file.\n";
	if (curl){
		CURLcode res;
		//struct curl_slist *chunk = NULL;

		//chunk = curl_slist_append(chunk,"Accept:image/jpeg");
		//chunk = curl_slist_append(chunk,"Accept:image/png");

		//curl_easy_setopt(curl,CURLOPT_HTTPHEADER, chunk);
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, 0);
		curl_easy_setopt(curl,CURLOPT_WRITEDATA,fp);
		curl_easy_setopt(curl,CURLOPT_URL,uri);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK){
			std::cout<<"curl_easy_perform() failed with code "<<res<<"\n";
			fclose(fp);
			return false;
		}
	}		
	//*(data.str+data.bytes) = '\0'; //Clear trailing characters
	std::cout << "Fetched filedata.\n";
	fclose(fp);
	return true;
}
