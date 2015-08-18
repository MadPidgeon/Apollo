#pragma once
#include <string>
#include <cstdint>
#include <set>
#include "notes.hh"

namespace theory {

class chord {
	std::set<note> _notes;
	note _root;
	uint16_t _fingerprint;
public:
	void addNote( const note& );
	void removeNote( const note& );
	note getRootNote() const;
	uint16_t getFingerprint() const;
	std::string getChordName() const;
};

};