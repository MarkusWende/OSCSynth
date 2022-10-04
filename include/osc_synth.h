#pragma once

#include <jackaudioio.hpp>
#include <algorithm>
#include <unistd.h>
#include <map>
#include <memory>

#include "oscicontainer.h"
#include "oscman.h"
#include "midiman.h"
#include "Biquad.h"
#include "sinusoid.h"
#include "distortion.h"
#include "adsr.h"

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
	std::map<int, std::unique_ptr<Oscicontainer>> osci;
	OscMan *osc;

	MidiMan *midi;
	Biquad *filter;
	Oscicontainer *lfo;
	Distortion* distortion;

	bool filterStatus;
	bool distortion_status_;

	jack_nframes_t fs;
	jack_nframes_t nframes;
	double gain_;

	// variables for midi handling
	double t_tracking; //time tracker
	int counter; //counter for the number of used oscillators
	size_t maxAnzahl_Osci;// max number of availible oscillators

	// vecors for midi handling
	std::vector<int> Noten;
	std::vector<int> freeOsci;
	std::vector<double> timetracker;

	// variables for osc handling
	double valOld;
	std::string typeOld;
	std::string pathOld;
	double lfo_oldValue = 0.0;

	// Ring buffer output
	JackCpp::RingBuffer<float>* ring_buffer_out_;

public:

    /// Declaration of Audio Callback Function:
    virtual int audioCallback(jack_nframes_t nframes,
                              // A vector of pointers to each input port.
                              audioBufVector inBufs,
                              // A vector of pointers to each output port.
                              audioBufVector outBufs); 

    /// Constructor
	OSCSynth();
	~OSCSynth();

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
	void SetGain(double gain) { gain_ = gain; };
	void process();

};