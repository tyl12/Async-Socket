
CFLAGS=-O2 -I${SDLFLAGS} -pthread -lcrypt -lboost_system

all:
	g++ server.cpp -o server Tools.h -I ./ -I lib/ lib/*.cpp ./utils.cpp -lpthread -std=c++11  ${CFLAGS}
	g++ client.cpp -o client -I ./ -I lib/ lib/*.cpp ./utils.cpp -lpthread -std=c++11  ${CFLAGS}

#./server
#./client
