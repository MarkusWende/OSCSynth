//class Noise
//
// Noise sginal is generated numerically with random values

#ifndef NOISE_H
#define NOISE_H

#include <stdlib.h>     /* srand, rand */

#define _USE_MATH_DEFINES

class Noise {
public:
    Noise(double a);
    void proceed(double ms);

    /// getters
    double amplitude();
    /// setters (override)
    void amplitude(double a);

    double getNextSample();
    double getCurrentAmpl();


private:

    double amp;
    double curr_ampl;

    // SYSTEM RELATED
    int nframes;
    int fs;

    double *t;


};

#endif // NOISE_H
