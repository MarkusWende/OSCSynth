//class Sawtoothwave
//
// sawtoothsginal is generated with sawtooth formula (no additive sinusoidal synthesis)

#include "sawtoothwave.h"

// Constructor
Sawtoothwave::Sawtoothwave(double f, double a, double p,  int fS)
{
    freq    = f;
    amp     = a;
    phi     = p;
    fs      = fS;
    curr_ampl = 0.0;
}

double Sawtoothwave::getNextSample() {

    /// This method gets the next sample of the Sawtoothwave.
    /// The amplitude is applied and the angle of the
    /// Sawtoothwave is increased, according to the sample rate.


    double n=2.0*M_PI * freq * (1.0/fs);
    
    //double thisVal=amp*((phi-fs/freq*n)   -(long)(phi-fs/freq*n));
    double thisVal=phi;
    
    // apply amplitude
    thisVal = amp*(thisVal-M_PI)/M_PI;
    
     // rotate to next step
    phi += n;
    if (phi>=2.0*M_PI) phi=0;

    curr_ampl = thisVal;

    return thisVal;

}

// getter methods

double Sawtoothwave::frequency() {
    return freq;
}

// method returns the non time-dependand (peak) amplitude
double Sawtoothwave::amplitude() {
    return amp;
}

double Sawtoothwave::phase() {
    return phi;
}

// method returns the current time-dependend amplitude of the oscillating signal 
double Sawtoothwave::getCurrentAmpl() {
    return curr_ampl;
}


// setter methods
void Sawtoothwave::frequency(double f) {
    freq = f;
}

void Sawtoothwave::amplitude(double a) {
    amp = a;
}

void Sawtoothwave::phase(double p) {
    phi = p;

}
