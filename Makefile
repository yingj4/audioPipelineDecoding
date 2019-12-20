default_target: all

SRCDIR=./src/
BINDIR=./bin/
CC=gcc
CFLAGS=-g -Wall -std=c++11
HEADERDIR=./include

CFiles=$(wildcard $(SRCDIR)*.cpp)
Objects=$(patsubst %.c,%.o,$(wildcard $(CFiles)))

all: $(Objects)
	$(CC) $(CFLAGS) $(Objects) -o audio -lspatialaudio -lstdc++ -lm -I$(HEADERDIR)

audio: $(CFiles)
	$(CC) $(CFLAGS) $(CFiles)

clean:
	rm -rf audio
