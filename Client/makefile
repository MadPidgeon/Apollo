GFLAG = -Wall -Wextra -pedantic -lboost_system -std=c++11
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	CXX = clang++
else
	CXX = g++
endif

all: client_connect.o RtMidiChoose.o

client_connect.o:
	$(CXX) -c client_connect.cc -o client_connect.o $(GFLAG)

RtMidiChoose.o:
	$(CXX) -c RtMidiChoose.cpp -o RtMidiChoose.o $(GFLAG)