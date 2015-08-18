#include <string>
#include <cstdint>
#include "notes.hh"

namespace theory {

uint8_t note::getNoteNumber() const {
	return _midi_number;
}

uint8_t note::getClassNumber() const {
	return _midi_number % 12;
}

void note::setNoteNumber( uint8_t num ) {
	_midi_number = num;
}

std::string note::getClassName() const {
	int n = ( getClassNumber() + 3 ) % 12;
    n += ( n >= 3 ) + ( n >= 8 );
    std::string r = " ";
    r.at(0) = char( 'A' + n/2 );
    if( n & 1 )
        r += '#';
    return r;
}

string note::getNoteFullName() const {
	return getClassName();
}

bool note::isBlack() const {
    int c = getClassNumber();
    if( c < 5 )
        return c & 1;
    else
        return !( c & 1 );
}

int note::operator-( const theory::note& other ) const {
	return getNoteNumber() - other.getNoteNumber();
}

note::note() {
	setNoteNumber( 60 );
}

note::note( uint8_t num ) {
	setNoteNumber( num );
}

};