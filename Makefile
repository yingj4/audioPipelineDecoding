SRCDIR=./src/
BINDIR=./bin/
CC=clang
CXX=clang++
LD=clang++
CFLAGS=-Wall -fPIC -I./include
CXXFLAGS=-std=c++2a -Wall -fPIC -I./include
LD_LIBS=-lpthread -pthread
LD_LIBS=-lpthread -pthread
DBG_FLAGS=-g -I./libspatialaudio/build/Debug/include
OPT_FLAGS=-O3 -I./libspatialaudio/build/Release/include

SRCFILES=audio.cpp sound.cpp
OBJFILES=$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRCFILES)))

.PHONY: clean deepclean

solo.dbg: CFLAGS += $(DBG_FLAGS)
solo.dbg: CXXFLAGS += $(DBG_FLAGS)
solo.dbg: LIBSPATIALAUDIO_BUILD_TYPE=Debug
solo.dbg: $(OBJFILES) main.o libspatialaudio/build/Debug/lib/libspatialaudio.a
	$(LD) $(DBG_FLAGS) $^ -o $@ $(LD_LIBS)

solo.opt: CFLAGS += $(OPT_FLAGS)
solo.opt: CXXFLAGS += $(OPT_FLAGS)
solo.opt: LIBSPATIALAUDIO_BUILD_TYPE=Release
solo.opt: $(OBJFILES) main.o libspatialaudio/build/Release/lib/libspatialaudio.a
	$(LD) $(OPT_FLAGS) $^ -shared -o $@ $(LD_LIBS)

%.o: src/%.cpp libspatialaudio/build
	$(CXX) $(CXXFLAGS) $< -c -o $@

%.o: src/%.c libspatialaudio/build
	$(CC) $(CFLAGS) $< -c -o $@

libspatialaudio/build/Debug/lib/libspatialaudio.a: libspatialaudio/build
libspatialaudio/build/Release/lib/libspatialaudio.a: libspatialaudio/build

libspatialaudio/build:
	mkdir -p libspatialaudio/build/$(LIBSPATIALAUDIO_BUILD_TYPE)
	cd libspatialaudio/build; \
	cmake -DCMAKE_INSTALL_PREFIX=$(LIBSPATIALAUDIO_BUILD_TYPE) \
	      -DCMAKE_BUILD_TYPE=$(LIBSPATIALAUDIO_BUILD_TYPE) ..
	$(MAKE) -C libspatialaudio/build
	$(MAKE) -C libspatialaudio/build install

clean:
	rm -rf audio *.o *.so solo.dbg solo.opt

deepclean: clean
	rm -rf libspatialaudio/build
