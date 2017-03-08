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



private:

    double amp;

    // SYSTEM RELATED
    int nframes;
    int fs;

    double *t;


};

#endif // NOISE_H