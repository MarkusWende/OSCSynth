#ifndef OSCICONTAINER_H
#define OSCICONTAINER_H

#include <stdint.h>
#include <iostream>

#include "sawtoothwave.h"
#include "squarewave.h"
#include "noise.h"
#include "sinusoid.h"
#include "releaseNote.h"
#include "adsr.h"

class Oscicontainer {
private:
	// audible signal objects
 	Sinusoid *osciSine;
	Sawtoothwave *osciSaw;
	Squarewave *osciSquare;
	Noise *osciNoise;
	
	// lfo signal objects
	Sawtoothwave *lfoSaw;
	Squarewave *lfoSquare;
	Sinusoid *lfoSin;

	// release Note object
	releaseNote *relNote;

	// ADSR object and status
	ADSR *envelope;
	bool ADSRStatus;

	// signal amplitudes
	double osciSawAmpl;
	double osciSquareAmpl;
	double osciNoiseAmpl;
	double osciSineAmpl;

	// is the container object a lfo
	bool isLFO;
	// lfo type
	int type;


public:
	// Constructor for the audible signal Container
	Oscicontainer(uint32_t fs);
	// Constructor for the lfo signal container
	Oscicontainer(int type, double f);
	

	// Setters
	void amplitude(double a);
	void frequency(double f);
	void phase(double phi);
	void setSawAmpl(double a);
	void setSquareAmpl(double a);
	void setNoiseAmpl(double a);
	void setSineAmpl(double a);
	void setReleaseNoteState(int status);
	void setLFOtype(int Type);
	void setADSRState(int status);
	void setADSRStatus(bool status);
	void setADSRAttackTime(float t);
	void setADSRDecayTime(float t);
	void setADSRSustainLevel(float level);
	void setADSRReleaseTime(float t);

	// Getters
    double getNextSample();
    double getCurrentAmpl();
};

#endif // OSCICONTAINER_H
