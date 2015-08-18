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

bool inErrorRange( double a, double b ) {
    return ( a / b > .9 and b / a > .9 );
}

int matchWithFactor( dt a, dt b, dt& r ) {
    int part;
    if( a.count() == 0 or b.count() == 0 )
        throw;
    if( a > b ) {
        part = double(a.count()) / b.count() + 0.5;
        r = a / part;
        if( inErrorRange( ( part * b ).count(), a.count() ) )
            return part;
    } else {
        part = double(b.count()) / a.count() + 0.5;
        r = a * part;
        if( inErrorRange( ( part * a ).count(), b.count() ) )
            return part;
    }
    return 0;
}

timeUnit timeDistance( const timePoint& a, const timePoint& b ) {
    if( a > b )
        return boost::chrono::duration_cast<timeUnit>( a - b );
    else
        return boost::chrono::duration_cast<timeUnit>( b - a );
}

int repeatedIncrement( timePoint& t, const dt& i, const timePoint& m ) {
    int c = 0;
    while( t < m ) {
        t += i;
        ++c;
    }
    return c;
}

class alignRithm {
private:
    deque<boost::chrono::high_resolution_clock::time_point> history;
    boost::chrono::high_resolution_clock::time_point lastWholeTime;
    boost::chrono::high_resolution_clock::time_point lastPredictedWholeTime;
    boost::chrono::high_resolution_clock::time_point nextPredictedWholeTime;
    dt wholeTime;
    int advancedPredictions;
    bool validState;

    void invalidateState();
    bool findWholeTime();
public:
    boost::mutex stateLock;
    boost::condition_variable stateCheck;

    bool valid() const;
    void tick();
    boost::chrono::high_resolution_clock::time_point predict();
    alignRithm();
};

alignRithm::alignRithm() {
    invalidateState();
}

void alignRithm::invalidateState() {
    cout << "Invalidating state!" << endl;
    validState = false;
    history.clear();
}

bool alignRithm::valid() const {
    return validState;
}

bool alignRithm::findWholeTime() {
    int historySize = history.size();
    dt roundedDifference;
    int factor;
    timePoint historyBegin, historyEnd;
    vector<dt> deltaTime;
    int maximumValue = 0;
    vector<double> value;
    if( historySize >=2 ) {
        historyBegin = history.front();
        historyEnd = history.back();
        if( historyEnd - historyBegin >= boost::chrono::seconds(4) ) {
            if( historyEnd - historyBegin >= boost::chrono::seconds(6) ) {
                history.clear();
                history.push_back( historyEnd );
                return false;
            }
            cout << "Calculating whole time!" << endl << flush;
            value.assign( historySize - 1, 0 );
            deltaTime.resize( historySize - 1 );
            for( int i = 0; i < historySize - 1; ++i ) {
                deltaTime[i] = boost::chrono::duration_cast<dt>( history[i+1] - historyBegin );
                if( deltaTime[i].count() == 0 ) {
                    cout << "Correcting for timing error!" << endl;
                    deltaTime[i] = boost::chrono::milliseconds(1);
                }
                cout << deltaTime[i].count() << "us  ";
            }
            cout << endl;
            for( int i = 0; i < historySize - 1; ++i ) {
                for( int j = i + 1; j < historySize - 1; ++j ) {
                    factor = matchWithFactor( deltaTime[i], deltaTime[j], roundedDifference );
                    cout << i << "," << j << ": " << factor << endl;
                    if( factor != 0 and factor != 1 and factor <= 8 ) {
                        ++value[i];
                        ++value[j];
                    }
                }
            }

            for( int i = 0; i < historySize - 1; ++i ) {
                cout << value[i] << "v ";
                value[i] -= float( abs( ( deltaTime[i] - boost::chrono::microseconds(1000000) ).count() ) ) / 1000000;
                cout << value[i] << "v ";
                if( value[i] > value[maximumValue] )
                    maximumValue = i;
            }
            cout << endl;
            wholeTime = deltaTime[ maximumValue ];
            nextPredictedWholeTime = history[ maximumValue + 1 ];
            repeatedIncrement( nextPredictedWholeTime, wholeTime, boost::chrono::high_resolution_clock::now() );
            lastWholeTime = nextPredictedWholeTime - wholeTime;
            lastPredictedWholeTime = lastWholeTime;
            cout << "WholeTime: " << wholeTime << endl;

            validState = true;
            return true;
        }
    }
    return false;
}

void alignRithm::tick() {
    boost::chrono::high_resolution_clock::time_point now = boost::chrono::high_resolution_clock::now();
    dt difference; 
    dt incr; 
    dt deltaTime, deltaTime2;
    dt roundedDifference;
    int part;
    //cout << now << ": tick!" << endl;
    if( valid() ) {
        /*difference = boost::chrono::duration_cast<dt>( currentTime - history.back() );
        part = matchWithFactor( difference, wholeTime, roundedDifference );
        if( part != 0 ) {
            wholeTime = ( history.size() * wholeTime + roundedDifference ) / ( history.size() + 1 );
            history.push_back( currentTime );
        } else {
            cout << "Odd stuff detected!" << endl;
            invalidateState();
        }
        cout << wholeTime << endl;*/
        //difference = boost::chrono::duration_cast<dt>( now - lastPredictedWholeTime );
        //factor = matchWithFactor( difference,  )

        //adjustWholeTime();

        /****************************************************/
        difference = boost::chrono::duration_cast<dt>( now - history.back() );
        part = matchWithFactor( difference, wholeTime, roundedDifference );
        cout << "part: " << part << endl;
        if( part != 0 and part <= 16 ) {
            if( difference > wholeTime )
                incr = difference / part;
            else
                incr = difference * part;
            if( incr >= 3*wholeTime / 4 and incr <= 5*wholeTime/4 )
                wholeTime = ( min( int(history.size() ), 12 ) * wholeTime + incr /*was rD*/ ) / min( int(history.size() + 1), 13 );
            cout << "wholeTime: " << wholeTime << endl;
            history.push_back( now );
            if( history.size() > 100 )
                history.erase( history.begin(), history.begin() + 50 );
        } /*else {
            cout << "Odd stuff detected!" << endl;
            invalidateState();
            return;
        }*/
        /****************************************************/

        if( timeDistance(now, lastPredictedWholeTime ) < boost::chrono::milliseconds( 100 ) ) { // now ~ lastPredictedWholeTime
            if( timeDistance( now, lastPredictedWholeTime ) < timeDistance( lastWholeTime, lastPredictedWholeTime ) ) {
                lastWholeTime = now;
                nextPredictedWholeTime = lastWholeTime + wholeTime;
            }
        }
        /****************************************************/
        boost::chrono::high_resolution_clock::time_point temp = nextPredictedWholeTime;
        int rep = repeatedIncrement( temp, wholeTime, now );
        if( timeDistance( now, temp ) > wholeTime / 2 )
            temp -= wholeTime;
        if( timeDistance( now, temp ) < boost::chrono::milliseconds( 100 ) ) {
            lastWholeTime = temp;
            lastPredictedWholeTime = now;
            nextPredictedWholeTime = lastWholeTime + wholeTime;
        }
        /****************************************************/
        /*
        if( timeDistance( now, nextPredictedWholeTime ) < boost::chrono::milliseconds( 100 ) ) {
            lastWholeTime = nextPredictedWholeTime;
            lastPredictedWholeTime = now;
            nextPredictedWholeTime = lastWholeTime + wholeTime;
        }*/

    } else {
        history.push_back( now );
        if( findWholeTime() ) {
            stateCheck.notify_one();
            advancedPredictions = 0;
        }
    }
}

boost::chrono::high_resolution_clock::time_point alignRithm::predict() {
    boost::chrono::high_resolution_clock::time_point now = boost::chrono::high_resolution_clock::now();
    boost::chrono::high_resolution_clock::time_point prediction = nextPredictedWholeTime;
    if( repeatedIncrement( prediction, wholeTime, now ) > 2 ) {
        cout << "Predicted to far ahead!" << endl;
        invalidateState();
    }
    if( prediction - now < wholeTime / 2 )
        prediction += wholeTime;
    return prediction;
}

void callbackMidiInput( chat_client* c, alignRithm* o ) {
    boost::chrono::high_resolution_clock::time_point waitUntil;
    boost::chrono::high_resolution_clock::duration waitFor;
    boost::posix_time::time_duration posixTime;
    dt microsecondsBoost;
    uint64_t microsecondsTime;
    boost::unique_lock<boost::mutex> lock( o->stateLock );

    while( true ) {
        while( !o->valid() ) {
            o->stateCheck.wait( lock );
        }
        waitUntil = o->predict();
        waitFor = waitUntil - boost::chrono::high_resolution_clock::now();
        microsecondsBoost = boost::chrono::duration_cast<boost::chrono::microseconds>(waitFor);
        microsecondsTime = microsecondsBoost.count();
        posixTime = boost::posix_time::microseconds( microsecondsTime );
        boost::this_thread::sleep( posixTime );
        if( o->valid() ) {
            boost::this_thread::disable_interruption di;
            c->write( c->my_channel() + 143, midi_time_tick, 0 );
            c->write( c->my_channel() + 143, midi_time_tick, 64 );
            //cout << "tock!" << endl;
        }
    }
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
            std::cerr << "Usage: rhythm_bot <host> <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(argv[1], argv[2]);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        chat_client c(io_service, iterator);

        alignRithm rithmObject;
        c.set_message_reactor( boost::bind( callbackMidiOutput, &rithmObject, _1 ) );
        //c.request_channel( 2 );
        c.request_channel( 10 );

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