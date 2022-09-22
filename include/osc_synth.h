#pragma once

#include <jackaudioio.hpp>
#include <algorithm>
#include <unistd.h>

#include "oscicontainer.h"
#include "oscman.h"
#include "midiman.h"
#include "Biquad.h"
#include "sinusoid.h"
#include "distortion.h"
#include "adsr.h"

using namespace std;

//Preset Numbers
enum presetnumber{
    wobble = 		1,
    dreamy = 		2,
    nice_pulse=		3,
    in_the_night=	4,
    high_hat=		5,


};


class OSCSynth: public JackCpp::AudioIO {

private:

	Oscicontainer **osci;
	OscMan *osc;

	MidiMan *midi;
	Biquad *filter;
	Oscicontainer *lfo;
	Distortion* distortion;

	bool filterStatus;

	jack_nframes_t fs;
	jack_nframes_t nframes;

	// variables for midi handling
	double t_tracking; //time tracker
	int counter; //counter for the number of used oscillators
	int maxAnzahl_Osci;// max number of availible oscillators

	// vecors for midi handling
	vector<int> Noten;
	vector<int> freeOsci;
	vector<double> timetracker;

	// variables for osc handling
	double valOld;
	string typeOld;
	string pathOld;
	double lfo_oldValue=0;

public:

    /// Declaration of Audio Callback Function:
    virtual int audioCallback(jack_nframes_t nframes,
                              // A vector of pointers to each input port.
                              audioBufVector inBufs,
                              // A vector of pointers to each output port.
                              audioBufVector outBufs); 

    /// Constructor
	OSCSynth();

	// Setters
	
 	void lfoHandler();
	void midiHandler();
	void oscHandler();
	void presets(int preset);

	void setAllSineAmpl(double val);
	void setAllSawAmpl(double val);
	void setAllSquareAmpl(double val);
	void setAllNoiseAmpl(double val);
	void setAllADSRStatus(int val);
	void setAllADSRSustainLevel(double val);
	void setAllADSRAttackTime(double val);
	void setAllADSRReleaseTime(double val);
	void setAllADSRDecayTime(double val);

};