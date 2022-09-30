// This class is a distortion filter
//  distortion.cpp

//This filter class  is general gain or distortion filter that is applied to the post-filtered signal. 
// The Schetzen formula is used to create distortion or gain

#include "distortion.h"


Distortion::Distortion() {
    gain=1;

}

Distortion::Distortion(int gain) {
    setGain(gain);

}

Distortion::~Distortion() {
}


void Distortion::setGain(int gain) {
    gain = gain;
}

