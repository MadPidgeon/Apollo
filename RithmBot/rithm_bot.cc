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

typedef boost::chrono::nanoseconds dt;

enum{ midi_time_tick = 60 };

bool inErrorRange( double a, double b ) {
    return ( a / b > .9 and b / a > .9 );
}

int matchWithFactor( dt a, dt b ) {
    int part;
    if( a > b ) {
        part = a / b;
        roundedDifference = a / part;
        if( inErrorRange( ( part * b ).count(), a.count() ) )
            return part;
    } else {
        part = b / a;
        roundedDifference = a * part;
        if( inErrorRange( ( part * a ).count(), b.count() ) )
            return part;
    }
    return 0;
}

class alignRithm {
private:
    deque<boost::chrono::high_resolution_clock::time_point> history;
    boost::chrono::high_resolution_clock::time_point lastWholeTime;
    boost::chrono::nanoseconds wholeTime;
    boost::mutex stateLock;
    bool validState;

    void invalidateState();
public:
    void tick();
    boost::chrono::high_resolution_clock::time_point predict() const;
    alignRithm();
};

alignRithm::alignRithm() {
    invalidateState();
}

void alignRithm::invalidateState() {
    validState = false;
    history.clear();
}

void alignRithm::tick() {
    boost::chrono::high_resolution_clock::time_point currentTime = boost::chrono::high_resolution_clock::now();
    boost::chrono::nanoseconds difference = currentTime - history.back();
    boost::chrono::nanoseconds deltaTime, deltaTime2;
    boost::chrono::nanoseconds roundedDifference;
    int part;
    bool succes = false;

    if( validState ) {
        /*if( difference > wholeTime ) {
            part = difference / wholeTime;
            roundedDifference = difference / part;
            if( inErrorRange( ( part * wholeTime ).count(), difference.count() ) )
                succes = true;
        } else {
            part = wholeTime / difference;
            roundedDifference = difference * part;
            if( inErrorRange( ( part * difference ).count(), wholeTime.count() ) )
                succes = true;
        }*/
        if( matchWithFactor( difference, wholeTime ) != 0 ) {
            wholeTime = ( history.size() * wholeTime + roundedDifference ) / ( history.size() + 1 );
            history.push_back( currentTime );
        } else {
            cout << "Odd stuff detected!" << endl;
            invalidateState();
        }
    } else {
        history.push_back( currentTime );
        int historySize = history.size();
        if( history.front() - history.back() > boost::chrono::seconds( 2 ) ) {
            if( historySize <= 2 )
                history.clear();
            else {
                double* v = new double[historySize];
                int v_max = 1;
                for( int i = 0; i < historySize; ++i )
                    v[i] = 0;
                for( int i = 1; i < historySize; ++i ) {
                    deltaTime = history.at(i) - history.at(0);
                    for( int j = i + 1; j < historySize; ++j ) {
                        deltaTime2 = history.at(j) - history.at(0);
                        if( matchWithFactor( deltaTime, deltaTime2 ) != 0 ) {
                            ++v[i];
                            ++v[j];
                        }
                    }
                }
                for( int i = 1; i < historySize; ++i ) {
                    v[i] *= float( abs( boost::chrono::duration_cast<boost::chrono::microseconds>( history.at(i) - history.at(0) ) ) ) / 1000000;
                    if( v[i] > v[v_max] )
                        v_max = i;
                }
                wholeTime = history.at(v_max) - history.at(0);
                lastWholeTime = history.at(v_max);
                while( lastWholeTime < history.back() )
                    lastWholeTime += wholeTime; 
                validState = true;
                delete [] v;
                boost::unlock( stateLock );
            }
        }
    }
}

boost::chrono::high_resolution_clock::time_point alignRithm::predict() const {
    return history.back() + wholeTime;
}

void callbackMidiInput( chat_client* c, alignRithm* o ) {
    boost::chrono::high_resolution_clock::time_point waitUntil;
    boost::chrono::high_resolution_clock::duration waitFor;
    boost::posix_time::time_duration posixTime;
    boost::chrono::microseconds microsecondsBoost;
    uint64_t microsecondsTime;

    while( true ) {
        while( !o->validState() ) {
            m_condition.wait( lock );
        }
        waitUntil = o->predict();
        waitFor = waitUntil - boost::chrono::high_resolution_clock::now();
        microsecondsBoost = boost::chrono::duration_cast<boost::chrono::microseconds>(waitFor);
        microsecondsTime = microsecondsBoost.count();
        posixTime = boost::posix_time::microseconds( microsecondsTime );
        boost::this_thread::sleep( posixTime );
        {
            boost::this_thread::disable_interruption di;
            c->write( c->my_channel() + 143, midi_time_tick, 0 );
            c->write( c->my_channel() + 143, midi_time_tick, 64 );
        }
    }
}

void callbackMidiOutput( alignRithm* o, vector<uint8_t>* v ) {
    if( v->size() == 3 and v->at(0) >= 144 and v->at(0) < 160 and v->at(2) != 0 )
        o->tick();
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
        c.request_channel( 2 );

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