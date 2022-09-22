//class Sinusoid
//
// Sinusoid signal is generated with sin function from math.h

#ifndef SINUSOID_H
#define SINUSOID_H

#include <math.h>

#define _USE_MATH_DEFINES

class Sinusoid {
public:
    Sinusoid(double f, double a, double p, int fS);
    void proceed(double ms);

    /// getters
    double frequency();
    double amplitude();
    double phase();
    /// setters (override)
    void frequency(double f);
    void amplitude(double a);
    void phase(double p);


    double getNextSample();
    double getCurrentAmpl();



private:

    // PURE SINE STATE
    double freq;
    double amp;
    double phi;
    double curr_ampl;

    // SYSTEM RELATED
    int nframes;
    int fs;

    double *t;

    bool midiNoteOff;

};

#endif // SINUSOID_H
