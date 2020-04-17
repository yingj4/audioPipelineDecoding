SRCDIR=./src/
BINDIR=./bin/
CC=clang
CXX=clang++
CFLAGS=-g -Wall -fPIC
CXXFLAGS=-std=c++2a -Wall -g -fPIC
HEADERDIR=./include

CFiles=$(wildcard $(SRCDIR)*.cpp)
Objects=$(patsubst %.c,%.o,$(wildcard $(CFiles)))

.PHONY: all clean

all: libaudio.so

libaudio.so: audio_component.cpp $(CFiles)
	$(CXX) $(CXXFLAGS) $^ -shared -o libaudio.so -I$(HEADERDIR) -lpthread -pthread -lspatialaudio

solo: $(Objects)
	$(CC) $(CFLAGS) $(Objects) -o audio -lspatialaudio -lstdc++ -lm -I$(HEADERDIR)

audio: $(CFiles)
	$(CC) $(CFLAGS) $(CFiles)

clean:
	rm -f *.o *.so
