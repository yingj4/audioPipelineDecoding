SRCDIR=./src/
BINDIR=./bin/
CC=clang
CXX=clang++
CFLAGS=-Wall -fPIC
CXXFLAGS=-std=c++2a -Wall -fPIC
HEADERDIR=./include

DBG_FLAGS=-g
OPT_FLAGS=-O3

CFiles=$(wildcard $(SRCDIR)*.cpp)
Objects=$(patsubst %.c,%.o,$(wildcard $(CFiles)))

DBG_SO_NAME=plugin.dbg.so
OPT_SO_NAME=plugin.opt.so
.PHONY: all clean

all: $(DBG_SO_NAME)



$(DBG_SO_NAME): dbg_so solo

$(OPT_SO_NAME): opt_so solo

dbg_so: audio_component.cpp $(CFiles)
	$(CXX) $(CXXFLAGS) $(DBG_FLAGS) $^ -shared -o $(DBG_SO_NAME) -I$(HEADERDIR)  -lpthread -pthread -lspatialaudio

opt_so: audio_component.cpp $(CFiles)
	$(CXX) $(CXXFLAGS) $(OPT_FLAGS) $^ -shared -o $(OPT_SO_NAME) -I$(HEADERDIR)  -lpthread -pthread -lspatialaudio


solo: $(Objects)
	$(CC) $(CFLAGS) $(Objects) -o audio -lspatialaudio -lstdc++ -lm -I$(HEADERDIR)

audio: $(CFiles)
	$(CC) $(CFLAGS) $(CFiles)

clean:
	rm -f audio *.o *.so
