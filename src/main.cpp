#include <signal.h>
#include <aixlog.hpp>

#include "osc_synth.h"

// while exit condition
bool done = false;

// Signal Handler function
void exitSigHandler(int s)
{
	LOG(INFO) << "Caught signal " << s << "... Bye!\n";
	done = true; 
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	auto sink_cout = std::make_shared<AixLog::SinkCout>(AixLog::Severity::trace);
    auto sink_file = std::make_shared<AixLog::SinkFile>(AixLog::Severity::trace, "date.log");
    AixLog::Log::init({sink_cout, sink_file});
    
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

		synth->process();
		//usleep(5);
    }

    synth->disconnectOutPort(0);		// Disconnecting ports
    synth->close();						// stop client
    delete synth;						// dele synth object
    
	return 0;
}

