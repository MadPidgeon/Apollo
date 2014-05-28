//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "./../Global/chat_message.hpp"
#include "./../Client/client_connect.h"
#include "midi_input.h"


#include <boost/function.hpp>

using boost::asio::ip::tcp;
using namespace std;

void callbackMidiInput( double dt, vector<uint8_t> *message, void* data ) {
    int messageSize = message->size();
    callbackData* cd = (callbackData*) data;
    uint16_t nd;
    bool change = false;
    if( messageSize == 3 ) {
        if( message->at(0) == 144 ) {
            if( message->at(2) == 0 )
                change = cd->releaseKey( message->at(1) );
            else
                change = cd->pressKey( message->at(1), message->at(2) );
        } else if( message->at(0) == 128 )
            change = cd->releaseKey( message->at(1) );
        else if( message->at(0) == 176 ) {
            if( message->at(1) == 64 ) {
                if( message->at(2) == 0 )
                    change = cd->releasePedal();
                else
                    change = cd->pressPedal();
            }
        }
    }
    if( change ) {
        nd = cd->getNameData();
        for( int f = 1, i = 0; i < 12; ++i, f <<= 1 ) {
            if( nd & f )
                cout << getNoteName( i ) << ( isBlackNote( i ) ? "" : " " );
            else
                cout << "- ";
        }
        cout << endl;
    }
}

void callbackMidiOutput( RtMidiOut* o, vector<uint8_t>* m ) {
    o->sendMessage(m);
}

int main(int argc, char* argv[])
{
    try {
        if (argc != 3) {
            std::cerr << "Usage: chat_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(argv[1], argv[2]);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        chat_client c(io_service, iterator);
        RtMidiIn* midiInput = new RtMidiIn;
        RtMidiOut* midiOutput = new RtMidiOut;
        callbackData cd( boost::bind( &chat_client::write, boost::ref( c ), _1, _2, _3 ) );

        if( chooseMidiPort( midiInput, midiOutput ) ) {
            midiInput->setCallback( &callbackMidiInput, (void*) &cd );
            midiInput->ignoreTypes( true, true, true );
        } else {
            cerr << "Error opening midi ports" << endl;
            return 0;
        }

        //boost::function<void(vector<uint8_t>*)> f = boost::bind( &RtMidiOut::sendMessage, boost::ref(*midiOutput), _1 );
        //c.set_message_reactor( boost::bind( &RtMidiOut::sendMessage, boost::ref(*midiOutput), _1 ) );
        c.set_message_reactor( boost::bind( callbackMidiOutput, midiOutput, _1 ) );

        boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

        /*char line[chat_message::max_body_length + 1];
        while (std::cin.getline(line, chat_message::max_body_length + 1)) {
            chat_message msg;
            msg.body_length(strlen(line));
            memcpy(msg.body(), line, msg.body_length());
            msg.encode_header();
            c.write(msg);
        }*/
        cin.ignore();
        cin.get();

        c.close();
        t.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}