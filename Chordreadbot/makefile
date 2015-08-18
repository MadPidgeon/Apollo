GFLAG = -Wall -Wextra -pedantic -std=c++0x

subsystem:
	cd ./../Client && $(MAKE)

metronome_bot: subsystem
	g++ -o ./../Binaries/metronome_bot metronome_bot.cc ./../Client/client_connect.o $(GFLAG) -lpthread -lboost_thread -lboost_chrono -lboost_system
