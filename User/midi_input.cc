#include <iostream>
#include <cstdlib>
#include <vector>
#include <set>
#include <cstdint>
#include <boost/function.hpp>
#include <RtMidi.h>
#include "./../Client/RtMidiChoose.h"
#include "midi_input.h"

using namespace std;

bool callbackData::pressKey( uint8_t k, uint8_t v ) {
    uint16_t name = ( 1 << (k%12) );
    if( k < 144 ) {
        if( usePedal and pedalHistory.count(k) )
            pedalHistory.erase(k);
        actionKey( myChannel, k, v );
        keypressData[ k/12 ] |= name;
        nameData |= name;
        return true;
    }
    return false;
}

bool callbackData::releaseKey( uint8_t k ) {
    uint16_t name = ( 1 << (k%12) );
    if( k < 144 ) {
        if( usePedal )
            pedalHistory.insert( k );
        else {
            keypressData[ k/12 ] &= ~name;
            actionKey( myChannel, k, 0 );
            for( int i = 0; i < 12; ++i )
                if( keypressData[i] & name )
                    return true;
            nameData &= ~name;
            return true;
        }
    }
    return false;
}

bool callbackData::releaseKey( const set<uint8_t>& ks ) {
    for( const uint8_t& k : ks ) {
        keypressData[ k/12 ] &= ~( 1 << (k%12) );
        actionKey( myChannel, k, 0 );
    }
    nameData = 0;
    for( int i = 0; i < 12; ++i )
        nameData |= keypressData[i];
    return !ks.empty();
}

bool callbackData::pressPedal() {
    usePedal = true;
    return false;
}

bool callbackData::releasePedal() {
    if( usePedal ) {
        usePedal = false;
        if( !pedalHistory.empty() ) {
            releaseKey( pedalHistory );
            pedalHistory.clear();
            return true;
        }
    }
    return false;
}

uint16_t callbackData::getNameData() const {
    return nameData;
}

void callbackData::setChannel( uint8_t c ) {
    myChannel = c;
}

callbackData::callbackData( boost::function<void(uint8_t,uint8_t,uint8_t)> act ) {
    for( int i = 0; i < 12; ++i ) 
        keypressData[ i ] = 0;
    nameData = 0;
    usePedal = false;
    actionKey = act;
    myChannel = 144;
}

string getNoteName( uint8_t n ) {
    n = (n+3) % 12;
    n += ( n >= 3 ) + ( n >= 8 );
    string r = " ";
    r.at(0) = char( 'A' + n/2 );
    if( n & 1 )
        r += '#';
    return r;
}

bool isBlackNote( uint8_t n ) {
    if( n < 5 )
        return n & 1;
    else
        return !( n & 1 );
}