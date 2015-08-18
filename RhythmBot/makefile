GFLAG = -Wall -Wextra -pedantic -std=c++0x

subsystem:
	cd ./../Client && $(MAKE)

rhythm_bot: subsystem
	g++ -o ./../Binaries/rhythm_bot rhythm_bot.cc ./../Client/client_connect.o $(GFLAG) -lpthread -lboost_thread -lboost_chrono -lboost_system
