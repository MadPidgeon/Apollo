#pragma once
#include <cstdint>
#include <string>

namespace theory {

class note {
	uint8_t _midi_number;
public:
	uint8_t getNoteNumber() const;
	void setNoteNumber( uint8_t );
	std::string getClassName() const;
	std::string getNoteFullName() const;

	int operator-( const theory::note& ) const;
	note();
	note( uint8_t );
};

};