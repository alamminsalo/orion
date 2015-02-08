#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>

class Player{
	protected:	
		std::string name;
		unsigned short id;
		std::string URI;
		std::string params;

		const char *getCmd();
	
	public:
		Player();
		Player(std::string newName, std::string newURI, unsigned short newId) : 
			name(newName), URI(newURI), id(newId) {};
		~Player();
		void setParams(std::string newparams){
			this->params = newparams;
		};
		void play(std::string streamname){
			system(getCmd());
		};
		std::string getName(){
			return name;
		};
		std::string getURI(){
			return URI;
		};
		std::string getParams(){
			return params;
		};
		unsigned short getId(){
			return id;
		};
};

#endif //PLAYER_H
