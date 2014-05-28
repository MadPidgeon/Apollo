GFLAG = -Wall -Wextra -pedantic -lboost_system -std=c++0x

user: midi cconnect
	g++ -o cchat chat_user.cc ./../Client/client_connect.o midi_input.o ./../../MIDI/rtmidi-2.1.0/RtMidi.o ./../../MIDI/rtmidi-2.1.0/RtMidiChoose.o $(GFLAG) -lpthread -lboost_thread -lasound 

midi:
	g++ -c midi_input.cc $(GFLAG)

cconnect:
	g++ -c ./../Client/client_connect.cc -o ./../Client/client_connect.o $(GFLAG)