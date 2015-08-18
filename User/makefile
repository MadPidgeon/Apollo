UNAME := $(shell uname)
CXXFLAGS = -Wall -Wextra -pedantic -lboost_system -std=c++0x -lboost_thread -lrtmidi -lpthread
ifeq ($(OS),Windows_NT)
	CXX = i686-w64-mingw32-g++
	CXXFLAGS += -lasound -static-libstdc++ -static-libgcc -I/usr/include -B/usr/lib
else
	ifeq ($(UNAME), Darwin)
		CXX = clang++
		CXXFLAGS += -framework CoreMIDI
	else #linux?
		CXX = g++
		CXXFLAGS += -lrtmidi -lpthread -lasound
	endif
endif

all: user

subsystem:
	cd ./../Client && $(MAKE)

user: subsystem midi_input.o
	mkdir -p ../Binaries
	$(CXX) -o ./../Binaries/user chat_user.cc ./../Client/client_connect.o midi_input.o ./../Client/RtMidiChoose.o $(CXXFLAGS)

midi_input.o:
	$(CXX) -c midi_input.cc $(CXXFLAGS)
