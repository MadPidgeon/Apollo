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

typedef boost::chrono::high_resolution_clock::time_point timePoint;
typedef boost::chrono::high_resolution_clock::duration timeUnit;
typedef boost::chrono::microseconds dt;

//enum{ midi_time_tick = 60 };
enum{ midi_time_tick = 75 };


void callbackMidiInput( chat_client* c, alignRithm* o ) {
    
}

void callbackMidiOutput( alignRithm* o, vector<uint8_t>* v ) {
    if( v->size() == 3 and v->at(0) >= 144 and v->at(0) < 160 and v->at(2) != 0 ) {
        o->tick();
    }
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

        alignRithm rithmObject;
        c.set_message_reactor( boost::bind( callbackMidiOutput, &rithmObject, _1 ) );
        c.request_channel( 3 );

        boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));
        boost::thread u(boost::bind( callbackMidiInput, &c, &rithmObject ) );

        cin.ignore();
        cin.get();

        c.close();
        t.join();
        u.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}