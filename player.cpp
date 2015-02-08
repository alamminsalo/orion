#include "player.h"

Player::Player(){
	this->name = "UNDEFINED";
	this->URI = "UNDEFINED";
	this->params = "UNDEFINED";
	this->id = 0;
}

Player::~Player(){
}

const char *Player::getCmd(std::string streamname){
	std::string tmpStr = "";	
	
	tmpStr += URI;
	tmpStr += " ";
	tmpStr += params;
	tmpStr += " ";
	tmpStr += streamname;

	return tmpStr.c_str();
}
