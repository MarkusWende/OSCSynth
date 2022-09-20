#include <signal.h>
#include "osc_synth.h"

// while exit condition
bool done = false;

// Signal Handler function
void exitSigHandler(int s) {
	printf("Caught signal %d ... Bye!\n",s);
	// set while exit condition true
	done = true; 

}

int main(int argc, char *argv[]) {
    
    // create synthesizer object/client
    OSCSynth *synth = new OSCSynth();

    // activate the client
    synth->start();

    // connect sine ports to stereo ports
    synth->connectToPhysical(0,0);		// connects out port 0 to physical destination port 0
    synth->connectToPhysical(0,1);		// connects out port 1 to physical destination port 1

	// Unix signal handling 
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = exitSigHandler;		// Set up the structure to specify the new action
	sigemptyset(&sigIntHandler.sa_mask);			// initialize and empty a signal set
	sigIntHandler.sa_flags = 0;						// Flags to modify the behavior of the handler, or 0
	sigaction(SIGINT, &sigIntHandler, NULL);

	// Main Program Loop
	while(!done) {
		// calls the midi handler to process midi events
		synth->midiHandler();
		// calls the osc handler to process osc events
		synth->oscHandler();
		// calls the lfo handler to calculate the filter coefficients
		synth->lfoHandler();
		
		usleep(5);
    }

    synth->disconnectOutPort(0);		// Disconnecting ports
    synth->close();						// stop client
    delete synth;						// dele synth object
    
	return 0;
}

