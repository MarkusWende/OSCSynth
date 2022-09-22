//
// This class is a distortion filter
//  distortion.h


#ifndef distortion_h
#define distortion_h

#include <iostream>
#include <cmath>

using namespace std;

class Distortion {
public:

	//constructors
    Distortion();
    Distortion(int gain);

    //destructor
    ~Distortion();

    //setter
    void setGain(int gain);

    // process method
    float process(float in);

    
protected:

    int gain;


};

// distorion filter function
inline float Distortion::process(float in) {
    float out = in;
    
	//Distortion Schetzen Formula
    out=(out/abs(out))*(1-exp(gain*out*out/abs(out)));   
    

    return out;
}

#endif // distortion_h
