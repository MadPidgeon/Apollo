#pragma once
#include <vector>
#include <set>
#include <cstdint>
#include <boost/function.hpp>
#include "./../../MIDI/rtmidi-2.1.0/RtMidi.h"
#include "./../../MIDI/rtmidi-2.1.0/RtMidiChoose.h"
//#include "chat_message.hpp"

using namespace std;

string getNoteName( uint8_t );
bool isBlackNote( uint8_t );

class callbackData {
    uint16_t keypressData[12];
    uint16_t nameData;
    set<uint8_t> pedalHistory;
    bool usePedal;
    boost::function<void(uint8_t,uint8_t,uint8_t)> actionKey;
    uint8_t myChannel;
public:
    bool pressKey( uint8_t k, uint8_t v );
    bool releaseKey( uint8_t k );
    bool releaseKey( const set<uint8_t>& k );
    bool pressPedal();
    bool releasePedal();
    uint16_t getNameData() const;
    void setChannel( uint8_t );
    callbackData( boost::function<void(uint8_t,uint8_t,uint8_t)> );
};