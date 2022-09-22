// Midi manager receives messages from RT-Midi class 
////////////////////////////////////////////////////////////////////////

//  The Code was edited in certain areas by Robert Pelzer and Markus Wende

#ifndef MIDIMAN_H
#define MIDIMAN_H

#include "datatypes.h"
#include <RtMidi.h>
#include <unistd.h>

class MidiMan {
public:

    //constructor
    MidiMan();
    //Dectructor
    ~MidiMan();


    void flushProcessedMessages();

    midiMessage get_rtmidi();

    void setVerbose();

private:
    // rtmidi
    RtMidiIn *midiin;
    int nBytes, i;
    double stamp;
    bool isVerbose;


};

#endif // MIDIMAN_H
