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
	std::cout<<"clear\t\t--\tClears all channel data, preserving them in the list\n";	
	std::cout<<"\n";
};

int main(int argc, const char **argv){
	ChannelManager cman;

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
		for (int i=2; argv[i]; i++)
			cman.add(argv[i]);
		cman.writeJSON(DATAURI);
	}
	else if (arg == "set"){
		if (!argv[2]){
			std::cout<<"Need argument.\n";
			printMenu();
			return 0;
		}
		cman.readJSON(DATAURI);
		cman.setAlert(argv[2],"on");
		cman.writeJSON(DATAURI);
	}
	else if (arg == "unset"){
		if (!argv[2]){
			std::cout<<"Need argument.\n";
			printMenu();
			return 0;
		}
		cman.readJSON(DATAURI);
		cman.setAlert(argv[2],"off");
		cman.writeJSON(DATAURI);
	}
	else if (arg == "list"){
		cman.readJSON(DATAURI);
		cman.printList();
	}
	else if (arg == "update"){
		cman.updateChannels(true);
	}
	else if (arg == "check"){
		if (!argv[2]){
			cman.checkStreams(true);
		}
		else if (argc == 3){
			cman.checkStream(cman.find(argv[2]),false);
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
		std::cout << "This will remove all your stored channel data. Are you sure? (y/n)\n";
		std::cin >> opt;
		if (opt == 'y' || opt == 'Y'){
			cman.readJSON(DATAURI);
			cman.clearData();
			cman.writeJSON(DATAURI);
		}
		else std::cout << "Canceled.\n";
	}
	else if (arg == "start"){
		std::cout << "Running as service mode..\n";
		while(true){
			cman.checkStreams(true);
			sleep(60);
		}
	}
	else if (arg == "clear"){
		std::cout << "Clearing data..\n";
		cman.clearData();
	}
	else printMenu();

	std::cout << "::PROGRAM_EXIT::\n";

	return 0;
}
