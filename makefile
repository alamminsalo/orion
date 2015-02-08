CPP = main.cpp channelmanager.cpp channel.cpp fileutils.cpp connector.cpp
OBJECTS =
LIBS = -lcurl
OPTIONS = -Wall -std=gnu++11
NAME = streamwatch

$(NAME):
	clang++ -o $(NAME) $(OPTIONS) $(CPP) $(LIBS)

clean:
	rm streamwatch
