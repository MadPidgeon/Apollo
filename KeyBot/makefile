GFLAG = -Wall -Wextra -pedantic -std=c++0x

subsystem:
	cd ./../Client && $(MAKE)

rithm_bot: subsystem
	g++ -o ./../Binaries/rithm_bot rithm_bot.cc ./../Client/client_connect.o $(GFLAG) -lpthread -lboost_thread -lboost_chrono -lboost_system
