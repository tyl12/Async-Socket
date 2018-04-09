
CFLAGS=-O2 -I${SDLFLAGS} -pthread

all:
	g++ server.cpp -o server Tools.h -I ./ -I lib/ lib/*.cpp -lpthread -std=c++11  ${CFLAGS} ${SDLLIBS}
	g++ client.cpp -o client -I ./ -I lib/ lib/*.cpp -lpthread -std=c++11

#./server
#./client
