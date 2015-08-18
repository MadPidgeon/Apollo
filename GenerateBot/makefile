GFLAG = -Wall -Wextra -pedantic -std=c++0x

subsystem:
	cd ./../Client && $(MAKE)

generate_bot: subsystem
	g++ -o ./../Binaries/generate_bot generate_bot.cc ./../Client/client_connect.o $(GFLAG) -lpthread -lboost_thread -lboost_chrono -lboost_system
