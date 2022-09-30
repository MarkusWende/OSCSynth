#include "oscicontainer.h"

/* Constructor
 * Oscillator container which stores the different signal types
 */
Oscicontainer::Oscicontainer(uint32_t fs)
{
  fs_ = fs;
  // initialize signals with preset values
 	osciSine = new Sinusoid(440,0.0,0,fs_);
	osciSaw = new Sawtoothwave(440,0.0,0,fs_);
	osciSquare = new Squarewave(440,0.0,0,fs_);
	osciNoise = new Noise(0.0);

  // set lfo status to false -> this is the signal container
  // for the audible signals not lfo
	isLFO = false;

	// create release Note Object, for artefact elimination
	relNote = new releaseNote();

  // create adsr object for envelope
  envelope = new ADSR();
  // default is: adsr off
  ADSRStatus = false;

  // preset signal amplitudes
 	osciSineAmpl = 0.0;
	osciSawAmpl = 0.0;
	osciSquareAmpl = 0.0;
	osciNoiseAmpl = 0.0;

}
/* LFO Constructor: type and frequency selectable. 
 * Type 0 (and any other non-defined): Sinus, 
 * Type 1: Saw, Type 2: Square
 */
Oscicontainer::Oscicontainer(uint32_t fs, int type, double f)
{
  fs_ = fs;
  // initialize lfo signals with preset values
  lfoSaw = new Sawtoothwave(f,1,0,48000);
  lfoSquare = new Squarewave(f,1,0,48000);
  lfoSin = new Sinusoid(f,1,0,48000);
  
  // presets for different lfo signals
  if(type ==1) {
    lfoSaw->amplitude(1);
    lfoSquare->amplitude(0);
    lfoSin->amplitude(0);
  }
  else if (type ==2) {
    lfoSaw->amplitude(0);
    lfoSquare->amplitude(1);
    lfoSin->amplitude(0);
  } else {
    lfoSaw->amplitude(0);
    lfoSquare->amplitude(0);
    lfoSin->amplitude(1);
  }

  // set isLFO true, because this container is the lfo signal
  // container
  isLFO = true;
}


/* the getNextSample Methode returns the next value
 * of a signal, regardless its an lfo or audible
 * signal container
 */
double Oscicontainer::getNextSample() {
  // initialize return value
  double thisVal = 0.0;

  // lfo signal
  if (isLFO==true) {
    if(type ==1) thisVal= lfoSaw->getNextSample();
    else if (type ==2) thisVal = lfoSquare->getNextSample();
    else thisVal= lfoSin->getNextSample();
  
  // audible signal
	} else {
    // add up all amplitudes
    thisVal = osciSine->getNextSample();
    thisVal = thisVal + osciSaw->getNextSample();
    thisVal = thisVal + osciSquare->getNextSample();
    thisVal = thisVal + osciNoise->getNextSample();
    // if adsr is activated, multiply envelope and signal
    if (ADSRStatus) {
      thisVal = thisVal * envelope->Process();
    } else {
      thisVal = thisVal * relNote->process();
    }
  }
    return thisVal;

}

/* set signal amplitudes for the complete
 * lfo container or the complete
 * audible signal container
 */
void Oscicontainer::amplitude(double a) {
  // lfo signal container
  if (isLFO==true) {
      if(type ==1) {
        lfoSaw->amplitude(1);
        lfoSquare->amplitude(0);
      lfoSin->amplitude(0);
    } else if (type ==2) {
      lfoSaw->amplitude(0);
      lfoSquare->amplitude(1);
      lfoSin->amplitude(0);
    } else {
      lfoSaw->amplitude(0);
      lfoSquare->amplitude(0);
      lfoSin->amplitude(1);
    }
  
  // audible signal container
  } else {
    osciSaw->amplitude(osciSawAmpl*a);
    osciSquare->amplitude(osciSquareAmpl*a);
    osciNoise->amplitude(osciNoiseAmpl*a);
    osciSine->amplitude(osciSineAmpl*a);
	}
}

/* set signal frequencies for the complete
 * lfo container or the complete
 * audible signal container
 */
void Oscicontainer::frequency(double f) {
  // lfo signal container
  if (isLFO==true) {
    lfoSaw->frequency(f);
    lfoSquare->frequency(f);
    lfoSin->frequency(f);

  // audible signal container
  } else {
    osciSaw->frequency(f);
    osciSquare->frequency(f);
    osciSine->frequency(f);
	}
}

/* set signal phase for the complete
 * audible signal container
 */
void Oscicontainer::phase(double phi) {
	osciSaw->phase(phi);
	osciSquare->phase(phi);
	osciSine->phase(phi);
}

/* set sinewave amplitude
 */
void Oscicontainer::setSineAmpl(double a) {
	osciSineAmpl = a;
}

/* set sawtooth wave amplitude
 */
void Oscicontainer::setSawAmpl(double a) {
	osciSawAmpl = a;
}

/* set squarewave amplitude
 */
void Oscicontainer::setSquareAmpl(double a) {
	osciSquareAmpl = a;
}

/* set noise amplitude
 */
void Oscicontainer::setNoiseAmpl(double a) {
	osciNoiseAmpl = a;
}

/* set the releaseNote state
 * states are: note_off, note_on, note_release
 * remeber, the releaseNote class is for signal artifact handling
 */
void Oscicontainer::setReleaseNoteState(int status) {
	relNote->gate(status);
}

/* set adsr state
 * states are:
 * note_off = 0
 * attack = 1
 * decay = 2
 * sustain = 3
 * release = 4
 */
void Oscicontainer::setADSRState(int status) {
  envelope->SetState(status);
}

/* set adsr status on or off
 */
void Oscicontainer::setADSRStatus(bool status) {
  ADSRStatus = status;
}

/* set lfo type
 * Type 0 (and any other non-defined): Sinus, 
 * Type 1: Saw, Type 2: Square
 */
void Oscicontainer::setLFOtype(int Type) {
	type = Type;
	Oscicontainer::amplitude(0);
}

/* return the current lfo amplitude, if the container is
 * a lfo container, else 0 because there is no need to return
 * the audible signal amplitude for all signals add up together yet
 */
double Oscicontainer::getCurrentAmpl() {
  if (isLFO==true) {
    if(type ==1) 
      return lfoSaw->getCurrentAmpl();
    else if (type ==2) 
      return lfoSquare->getCurrentAmpl();
    else 
      return lfoSin->getCurrentAmpl();
  } else {
    return 0;
	}
}

/* set adsr attack_time
 * range: 1 to 99
 */
void Oscicontainer::setADSRAttackTime(float t) {
  envelope->SetAttack(t);
}

/* set adsr decay_time
 * range: 1 to 99
 */
void Oscicontainer::setADSRDecayTime(float t) {
  envelope->SetDecay(t);
}

/* set adsr sustain_level
 * range: 1 to 99
 */
void Oscicontainer::setADSRSustainLevel(float level) {
  envelope->SetSustain(level);
}

/* set adsr release_time
 * range: 1 to 99
 */
void Oscicontainer::setADSRReleaseTime(float t) {
  envelope->SetRelease(t);
}