#! /bin/sh


SDLLIBS=$(sdl-config --libs)
SDLFLAGS=$(sdl-config --cflags)
CFLAGS="-DUSE_SDL -O2 -I${SDLFLAGS} -pthread"

g++ server.cpp -o server Tools.h -I ../../lib/ ../../lib/*.cpp -lpthread -std=c++11  ${CFLAGS} ${SDLLIBS}
#./server
