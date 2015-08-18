GFLAG = -Wall -Wextra -pedantic -lboost_system -std=c++11

all: client_connect.o RtMidiChoose.o

client_connect.o:
	g++ -c client_connect.cc -o client_connect.o $(GFLAG)

RtMidiChoose.o:
	g++ -c RtMidiChoose.cpp -o RtMidiChoose.o $(GFLAG)