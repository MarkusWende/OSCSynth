#include "oscicontainer.h"

Oscicontainer::Oscicontainer() {
  osciSine = new Sinusoid(440,0.0,0,48000);
	osciSaw = new Sawtoothwave(440,0.0,0,48000);
	osciSquare = new Squarewave(440,0.0,0,48000);
	osciNoise = new Noise(0.0);

 osciSineAmpl = 1.0;
	osciSawAmpl = 1.0;
	osciSquareAmpl = 1.0;
	osciNoiseAmpl = 0.3;
}

double Oscicontainer::getNextSample() {


	double thisVal = osciSaw->getNextSample();
	thisVal = thisVal + osciSquare->getNextSample();
	thisVal = thisVal + osciNoise->getNextSample();
	thisVal = thisVal + osciSine->getNextSample();
	

    return thisVal;

}

void Oscicontainer::amplitude(double a) {
	osciSaw->amplitude(osciSawAmpl*a);
	osciSquare->amplitude(osciSquareAmpl*a);
	osciNoise->amplitude(osciNoiseAmpl*a);
	osciSine->amplitude(osciSineAmpl*a);
}

void Oscicontainer::frequency(double f) {
	osciSaw->frequency(f);
	osciSquare->frequency(f);
	osciSine->frequency(f);
}

void Oscicontainer::setSineAmpl(double a) {
	osciSineAmpl = a;
}
void Oscicontainer::setSawAmpl(double a) {
	osciSawAmpl = a;
}

void Oscicontainer::setSquareAmpl(double a) {
	osciSquareAmpl = a;
}

void Oscicontainer::setNoiseAmpl(double a) {
	osciNoiseAmpl = a;
}
