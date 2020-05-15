SRCDIR=./src/
BINDIR=./bin/
CC=clang
CXX=clang++
CFLAGS=-Wall -fPIC
CXXFLAGS=-std=c++2a -Wall -fPIC
HEADERDIR=./include

CFiles=$(wildcard $(SRCDIR)*.cpp)
Objects=$(patsubst %.c,%.o,$(wildcard $(CFiles)))

.PHONY: all clean

all: debug

debug: CXXFLAGS+=-g
debug: CFLAGS+=-g
debug: libaudio.so solo

release: CXXFLAGS+=-O3 
release: CFLAGS+=-O3
release: libaudio.so solo

libaudio.so: audio_component.cpp $(CFiles)
	$(CXX) $(CXXFLAGS) $^ -shared -o libaudio.so -I$(HEADERDIR) -lpthread -pthread -lspatialaudio

solo: $(Objects)
	$(CC) $(CFLAGS) $(Objects) -o audio -lspatialaudio -lstdc++ -lm -I$(HEADERDIR)

audio: $(CFiles)
	$(CC) $(CFLAGS) $(CFiles)

clean:
	rm -f *.o *.so
