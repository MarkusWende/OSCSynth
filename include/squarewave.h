//class Squarewave
//
// square sginal is generated with  formula (no additive sinusoidal synthesis)

#ifndef SQUAREWAVE_H
#define SQUAREWAVE_H

#include <math.h>

#define _USE_MATH_DEFINES

class Squarewave {
public:
    Squarewave(double f, double a, double p, int fS);
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


};

#endif // SQUAREWAVE_H
