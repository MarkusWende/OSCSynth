//
//  Biquad.h
//
//  Created by Nigel Redmon on 11/24/12
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the Biquad code:
//  http://www.earlevel.com/main/2012/11/25/biquad-c-source-code/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code
//  for your own purposes, free or commercial.
//
//////////////////////////////////////////////////////////////////////////////////////

//	The Code was edited in certain areas by Robert Pelzer and Markus Wende
// 	Where the Code was edited, comments were created

#ifndef Biquad_h
#define Biquad_h
#include <math.h>

enum {
    bq_type_lowpass = 0,
    bq_type_highpass=1,
    bq_type_bandpass=2,
    bq_type_notch=3,
    bq_type_peak=4,
    bq_type_lowshelf=5,
    bq_type_highshelf=6
};

class Biquad {
public:
    Biquad();
    Biquad(int type, double Fc, double Q, double peakGainDB);
    ~Biquad();
    void setType(int type);
    void setQ(double Q);
    void setFc(double Fc);
    void setPeakGain(double peakGainDB);
    void setGainReduce(int on);
    void setBiquad(int type, double Fc, double Q, double peakGain);
    float process(float in);
    void status();
    
protected:
    void calcBiquad(void);

    int type;
    double a0, a1, a2, b1, b2;
    double Fc, Q, peakGain;
    double z1, z2;
    bool gain_reduce=false;
};

inline float Biquad::process(float in) {
    double out = in * a0 + z1;
    z1 = in * a1 + z2 - b1 * out;
    z2 = in * a2 - b2 * out;
    
    // 	Section to reduce gain in areas where peak gain was applied earlier. 
    //	This leads to a reduction of the volume, however a high peakGain then works similar to the Q factor
    if(gain_reduce) {
    	out = out / pow(10, peakGain/20);
    }
    
    return out;
}

#endif // Biquad_h
