#include <string>
#include <set>
#include "notes.hh"

namespace theory {

void addNote( const note& n ) {
	_notes.insert( n );
}

void removeNote( const note& n ) {
	_notes.erase( n );
}

note getRootNote() const {
	return _root;
}

uint16_t getFingerprint() const {
	return _fingerprint;
}

std::string getChordName() const {
	theory::note rootNote = getRootNote();
	uint16_t fingerPrint = getFingerprint();
	std::string name = rootNote.getClassName();
	shift( fingerPrint, -rootNote.getClassNumber() );
}

};