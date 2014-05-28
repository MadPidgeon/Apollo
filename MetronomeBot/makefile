GFLAG = -Wall -Wextra -pedantic -std=c++0x

metronome: cconnect
	g++ -o metronome_bot metronome_bot.cc ./../Client/client_connect.o $(GFLAG) -lpthread -lboost_thread -lboost_chrono -lboost_system

cconnect:
	g++ -c ./../Client/client_connect.cc -o ./../Client/client_connect.o $(GFLAG)