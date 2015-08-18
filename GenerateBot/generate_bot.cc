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


#define _C		60
#define _D		62
#define _E		64
#define _F		65
#define _G		67
#define _A		69
#define _B		71

const int noteListSize = 7;
const uint8_t noteList[ noteListSize ] = { _C, _G, _E, _F, _A, _D, _B };
const int noteIntervalList[ noteListSize ] = { 1, 2, 3, 5, 7, 11, 13 };

int currentList = -1;
int currentTime = 0;
int currentNote[ noteListSize ]; 

timePoint faketime;

inline void wait() {
	faketime += boost::chrono::seconds( 1 );
	timeUnit timeInterval = faketime - boost::chrono::high_resolution_clock::now();
    dt microsecondsBoost = boost::chrono::duration_cast<boost::chrono::microseconds>( timeInterval );
    uint64_t microsecondsTime = microsecondsBoost.count();
    boost::posix_time::time_duration posixTime = boost::posix_time::microseconds( microsecondsTime );
    boost::this_thread::sleep( posixTime );
}

void play( chat_client* client ) {
	bool trigger;
	while( currentList < noteListSize ) {
		wait();
		trigger = true;
		for( int i = 0; i < currentList; ++i ) {
			currentNote[i]++;
			if( currentNote[i] >= noteIntervalList[i] ) {
				currentNote[i] = 0;
				cout << i << " ";
				client->write( client->my_channel() + 143, noteList[i], 64 );
			} else
				trigger = false;
		}
		cout << endl;
		if( trigger )
			currentNote[ currentList++ ] = 0;
		currentTime++;
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
        c.request_channel( 3 );

        faketime = boost::chrono::high_resolution_clock::now();

        boost::thread t(boost::bind( &boost::asio::io_service::run, &io_service ) );
        boost::thread u(boost::bind( play, &c ) );

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