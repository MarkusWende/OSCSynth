//class Sawtoothwave
//
// sawtoothsginal is generated with sawtooth formula (no additive sinusoidal synthesis)

#ifndef SAWTOOTHWAVE_H
#define SAWTOOTHWAVE_H

#include <cmath>

#define _USE_MATH_DEFINES

class Sawtoothwave {
private:

    // PURE SAWTOOTH STATE
    double freq;
    double amp;
    double phi;
    double curr_ampl;


    // SYSTEM RELATED
    int nframes;
    int fs;

    double *t;
    


public:
    Sawtoothwave(double f, double a, double p, int fS);
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

};

#endif // SQUAREWAVE_H
