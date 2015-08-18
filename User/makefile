GFLAG = -Wall -Wextra -pedantic -lboost_system -std=c++0x

all: user

subsystem:
	cd ./../Client && $(MAKE)

user: subsystem midi_input.o
	g++ -o ./../Binaries/user chat_user.cc ./../Client/client_connect.o midi_input.o ./../Client/RtMidiChoose.o $(GFLAG) -lrtmidi -lpthread -lboost_thread -lasound 

windowsUser: subsystem midi_input.o
	i686-w64-mingw32-g++ -o ./../Binaries/windowsUser.exe chat_user.cc ./../Client/client_connect.o midi_input.o ./../Client/RtMidiChoose.o $(GFLAG) -lrtmidi -lpthread -lboost_thread -lasound -static-libstdc++ -static-libgcc -I/usr/include -B/usr/lib

midi_input.o:
	g++ -c midi_input.cc $(GFLAG)
