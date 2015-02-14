CPP = main.cpp channelmanager.cpp channel.cpp fileutils.cpp connector.cpp threadman.cpp
OBJECTS = main.o channelmanager.o channel.o fileutils.o connector.o threadman.o
LIBS = -lcurl -pthread
OPTIONS = -std=c++11
NAME = streamwatch

#$(NAME): $(OBJECTS)
	#clang++ -o $(NAME) $(OBJECTS) $(LIBS)

#$(OBJECTS):
	#clang++ -c $(OPTIONS) $(CPP)
$(NAME):
	clang++ -o $(NAME) $(CPP) $(OPTIONS) $(LIBS)

clean:
	rm $(NAME)
