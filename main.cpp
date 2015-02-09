#include "channelmanager.h"
#include "global.h"
#include <iostream>
#include <unistd.h>


void printMenu(){
	std::cout<<"streamwatch is a small service to notify user about twitch channels and their state\n";
	std::cout<<"\n";
	std::cout<<"Syntax: streamwatch [OPTION]\n";
	std::cout<<"\nOptions:\n";
	std::cout<<"start\t\t--\tStarts watch server\n";	
	std::cout<<"add <id>\t--\tAdds a new channel by name\n";	
	std::cout<<"remove <id>\t--\tRemove a channel by name\n";	
	std::cout<<"list\t\t--\tLists all channels\n";	
	std::cout<<"update\t\t--\tUpdates data of all channels\n";	
	std::cout<<"check\t\t--\tChecks stream state of all channels\n";	
	std::cout<<"check <id>\t--\tChecks stream state by channel name\n";	
	std::cout<<"remove-all\t--\tRemoves all channels\n";	
	std::cout<<"set <id>\t--\tEnables channel notification alert\n";	
	std::cout<<"unset <id>\t--\tDisables channel notification alert\n";	
	std::cout<<"\n";
};

ChannelManager cman;

void run(){
	std::cout << "Running as service mode..\n";
	while(true){
		cman.readJSON(DATAURI);
		cman.checkStreams();
		sleep(60);
	}
};

int main(int argc, const char **argv){
	if (argc == 1){
		printMenu();
		return 0;
	}


	std::string arg(argv[1]);
	if (arg == "add"){
		if (!argv[2]){
			std::cout<<"Need argument.\n";
			printMenu();
			return 0;
		}
		cman.readJSON(DATAURI);
		cman.add(argv[2]);
		cman.writeJSON(DATAURI);
	}
	else if (arg == "set"){
		if (!argv[2]){
			std::cout<<"Need argument.\n";
			printMenu();
			return 0;
		}
		cman.readJSON(DATAURI);
		cman.setAlert(argv[2],true);
		cman.writeJSON(DATAURI);
	}
	else if (arg == "unset"){
		if (!argv[2]){
			std::cout<<"Need argument.\n";
			printMenu();
			return 0;
		}
		cman.readJSON(DATAURI);
		cman.setAlert(argv[2],false);
		cman.writeJSON(DATAURI);
	}
	else if (arg == "list"){
		cman.readJSON(DATAURI);
		cman.printList();
	}
	else if (arg == "update"){
		cman.readJSON(DATAURI);
		cman.updateChannels();
		cman.writeJSON(DATAURI);
	}
	else if (arg == "check"){
		if (!argv[2]){
			cman.readJSON(DATAURI);
			cman.checkStreams();
			cman.writeJSON(DATAURI);
		}
		else if (argc == 3){
			cman.readJSON(DATAURI);
			cman.checkStream(cman.find(argv[2]));
			cman.writeJSON(DATAURI);
		}
		else printMenu();
	}
	else if (arg == "remove"){
		if (!argv[2]){
			std::cout<<"Need argument.\n";
			printMenu();
			return 0;
		}
		cman.readJSON(DATAURI);
		cman.remove(argv[2]);
		cman.writeJSON(DATAURI);
	}
	else if (arg == "remove-all"){
		char opt;
		std::cout << "\nThis will remove all your stored channel data. Are you sure? (y/n)\n";
		std::cin >> opt;
		if (opt == 'y' || opt == 'Y'){
			cman.readJSON(DATAURI);
			cman.removeAll();
			cman.writeJSON(DATAURI);
		}
		else std::cout << "Canceled.\n";
	}
	else if (arg == "start"){
		run();
	}
	else printMenu();

	return 0;
}
