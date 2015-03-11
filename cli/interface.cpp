#include "interface.h"

Interface::Interface(){
	initscr();
	channelwin = NULL;

	for (int i=0; i<10; i++){
		std::string str = "item";
		items.push_back(str.c_str());
	}
}

Interface::~Interface(){
	endwin();
}

void Interface::drawList(){
	channelwin = newwin(0, 0, 0, 0);
	for (const char* str : items){
		wprintw(channelwin, str);
		wprintw(channelwin, "\n");
	}
	//box(channelwin, 0,0);
	wrefresh(channelwin);
}

void Interface::draw(){
	refresh();
	drawList();
}

void Interface::add(){
	
}
