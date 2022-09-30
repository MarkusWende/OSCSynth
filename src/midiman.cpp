// Midi manager receives messages from RT-Midi class 
///////////////////////////////////////////////////////////////////////

//  The Code was edited in certain areas by Robert Pelzer and Markus Wende

#include "midiman.h"

#include <aixlog.hpp>

std::vector<unsigned char>  a;
// vector to buffer the incoming data from rt midi
std::vector<unsigned char>  buffer;
// vector to safe the time instances
std::vector<double>  Zeit;

MidiMan::MidiMan()
{

    // rtmidid intit
	RtMidi::Api api = RtMidi::UNSPECIFIED;
	const std::string clientName = std::string("RtMidi Input Client");
	unsigned int queueSizeLimit = 100;

	midiin = new RtMidiIn(api,clientName,queueSizeLimit);	

    //
    //unsigned int nPorts = midiin->getPortCount();

    midiin->openPort( 0 );
    // Don't ignore sysex, timing, or active sensing messages.
    midiin->ignoreTypes( false, false, false );
	isVerbose = false;

    LOG(INFO) << "Started Midi Server!\n";

}

void
MidiMan::setVerbose()
{
    isVerbose = true;
}

midiMessage
MidiMan::get_rtmidi()
{
	midiMessage mm = {-1,-1,-1, 0, false};
    
    int nBytes = 1;

    /// this is kind of a dirty workaroud -
    /// we loop over all in-messages and take the last one:
    while(nBytes>0) {
		
		stamp=midiin->getMessage(&a);
		nBytes = a.size();

        /// only do something if bytes are received
        if (nBytes!=0)
        {      

			/// only give feedback if 'verbose-mode' is active
            if (isVerbose == true)
            {
                LOG(DEBUG) << "received " << nBytes << "Bytes: " ;

                for (int i=0; i<nBytes; i++ )
                    LOG(DEBUG) <<  i << " = " << (int)a[i] << " -- " ;

            }

            // fill the buffer and the time vector by inserting new instances in the beginning
            buffer.insert(buffer.begin(), {a[0], a[1], a[2]});
            Zeit.insert(Zeit.begin(), stamp);

        }

	sleep(0.01);
    
	}

    //	when the buffer holds at least 1 entry which consits of 3 values
    //	the MidiMessage is loaded with the oldest available values which are then being deleted
    if(buffer.size()>=3) {
		mm.byte3 = buffer.back();
		buffer.pop_back();
		mm.byte2 = buffer.back();
		buffer.pop_back();
		mm.byte1 = buffer.back();
		buffer.pop_back();
		mm.stamp=Zeit.back();
		Zeit.pop_back();

    }
    
    return mm;
}




//////////////////////////////////////////////////////////////////
// FLUSH the buffer
//////////////////////////////////////////////////////////////////

void
MidiMan::flushProcessedMessages()
{
   buffer.clear();
}

