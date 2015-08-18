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
#include <boost/thread.hpp>
//#include <boost/thread/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "./../Global/chat_message.hpp"
#include "./../Client/client_connect.h"

#include <boost/function.hpp>

using boost::asio::ip::tcp;
using namespace std;

enum{ midi_time_tick = 75 };

void callbackMidiInput( chat_client* c, bool randOmit, boost::posix_time::time_duration tpb ) {
    while( true ) {
        boost::this_thread::sleep( tpb );
        {
            boost::this_thread::disable_interruption di;
            if( (!randOmit) || rand() % 10 != 0 ) {
                c->write( c->my_channel() + 143, midi_time_tick, 0 );
                c->write( c->my_channel() + 143, midi_time_tick, 64 );
            }
        }
    }
}

void callbackMidiOutput( vector<uint8_t>* ) {
}

int main(int argc, char* argv[])
{
    try {
        if (argc != 3) {
            std::cerr << "Usage: x <host> <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(argv[1], argv[2]);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        chat_client c(io_service, iterator);

        uint64_t microsecondsInteger;
        char randOmit = 'n';
        boost::posix_time::time_duration tpb;
        cout << "Enter microseconds per beat (def=500000): ";
        cin >> microsecondsInteger;
        cout << "Ommit random tick? [y/N]";
        cin.ignore();
        cin.get(randOmit);

        tpb = boost::posix_time::microseconds( microsecondsInteger );

        c.set_message_reactor( callbackMidiOutput );
        c.request_channel( 10 );
        // c.request_instrument(  )

        cout << "Channel: " << int( c.my_channel() ) << endl;

        boost::thread t( boost::bind(&boost::asio::io_service::run, &io_service));
        boost::thread u( boost::bind( callbackMidiInput, &c, randOmit == 'y', tpb ) );

        cin.ignore();
        cin.get();
        
        u.interrupt();
        c.write( c.my_channel() + 143, midi_time_tick, 0 );
        c.close();
        t.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}