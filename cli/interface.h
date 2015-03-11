#ifndef INTERFACE_H
#define INTERFACE_H

#include <ncurses.h>
#include <vector>
#include <string>
//#include "channelmanager.h"

class Interface{
	WINDOW *channelwin;

	std::vector<const char*> items;

	void drawList();

	public:
		Interface();
		~Interface();

		void draw();
//		void add(Channel*);
//		void remove(Channel*);
		void add();
//		void remove();
//		void update();
};

#endif
