#pragma once
#include "RtMidi.h"
// This function should be embedded in a try/catch block in case of
// an exception.  It offers the user a choice of MIDI ports to open.
// It returns false if there are no ports available.
bool chooseMidiPort( RtMidiIn *rtmidi );
bool chooseMidiPort( RtMidiOut *rtmidi );
bool chooseMidiPort( RtMidiIn *rtmidiin, RtMidiOut *rtmidiout );