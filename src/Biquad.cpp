/**
 * @file adsr.h
 * @author Markus Wende and Robert Pelzer
 * @brief Biquad class implementation.
 */

//  Created by Nigel Redmon on 11/24/12
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the Biquad code:
//  http://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code
//  for your own purposes, free or commercial.
////////////////////////////////////////////////////////////////////////

//  The Code was edited in certain areas by Robert Pelzer and Markus Wende
//  Where the Code was edited, comments were created

#include "Biquad.h"

#include <iostream> 
#include <unistd.h>
#include <aixlog.hpp>

Biquad::Biquad()
{
    type_ = filterType::LOWPASS;
    a0_ = 1.0;
    a1_ = a2_ = b1_ = b2_ = 0.0;
    fc_ = 0.50;
    q_ = 0.0;
    SetPeakGain(0.0);
    z1_ = z2_ = 0.0;
}

Biquad::Biquad(int type, double fc, double q, double peakGain)
{
    type_ = type;
    a0_ = 1.0;
    a1_ = a2_ = b1_ = b2_ = 0.0;
    fc_ = fc;
    q_ = q;
    SetPeakGain(peakGain);
    z1_ = z2_ = 0.0;
}

Biquad::~Biquad()
{
}

void
Biquad::SetType(filterType type)
{
    type_ = type;
    calc_biquad();

    //  reduce gain for peak, highshelf and lowshelf
    //  This leads to a reduction of the volume, however a high peakGain then works similar to the Q factor
    if(type_ == filterType::PEAK || type_ == filterType::LOWSHELF || type_ == filterType::HIGHSHELF)
        SetGainReduce(1); 
    else
        SetGainReduce(0);
}

void
Biquad::SetQ(double Q)
{
    q_ = Q;
    calc_biquad();
}

void
Biquad::SetFc(double Fc)
{
    fc_ = Fc;
    calc_biquad();
}

void
Biquad::Status()
{
    LOG(INFO) << "Filter Type: " << type_ << "\n";
}

void
Biquad::SetPeakGain(double peakGain)
{
    peak_gain_ = peakGain;
    calc_biquad();
}

// turn on the gain reduction for applicable filter types
void
Biquad::SetGainReduce(bool status)
{
    if (status)
        gain_reduce_ = true;  
    else
        gain_reduce_ =false;
}

void
Biquad::calc_biquad(void) {
    double norm;
    auto V = std::pow(10, std::fabs(peak_gain_) / 20.0);
    auto K = std::tan(M_PI * fc_);
    switch (type_)
    {
        case filterType::LOWPASS:
            norm = 1 / (1 + K / q_ + K * K);
            a0_ = K * K * norm;
            a1_ = 2 * a0_;
            a2_ = a0_;
            b1_ = 2 * (K * K - 1) * norm;
            b2_ = (1 - K / q_ + K * K) * norm;
            break;
            
        case filterType::HIGHPASS:
            norm = 1 / (1 + K / q_ + K * K);
            a0_ = 1 * norm;
            a1_ = -2 * a0_;
            a2_ = a0_;
            b1_ = 2 * (K * K - 1) * norm;
            b2_ = (1 - K / q_ + K * K) * norm;
            break;
            
        case filterType::BANDPASS:
            norm = 1 / (1 + K / q_ + K * K);
            a0_ = K / q_ * norm;
            a1_ = 0;
            a2_ = -a0_;
            b1_ = 2 * (K * K - 1) * norm;
            b2_ = (1 - K / q_ + K * K) * norm;
            break;
            
        case filterType::NOTCH:
            norm = 1 / (1 + K / q_ + K * K);
            a0_ = (1 + K * K) * norm;
            a1_ = 2 * (K * K - 1) * norm;
            a2_ = a0_;
            b1_ = a1_;
            b2_ = (1 - K / q_ + K * K) * norm;
            break;
            
        case filterType::PEAK:
            if (peak_gain_ >= 0)
            {    // boost
                norm = 1 / (1 + 1/q_ * K + K * K);
                a0_ = (1 + V/q_ * K + K * K) * norm;
                a1_ = 2 * (K * K - 1) * norm;
                a2_ = (1 - V/q_ * K + K * K) * norm;
                b1_ = a1_;
                b2_ = (1 - 1/q_ * K + K * K) * norm;
            }
            else
            {    // cut
                norm = 1 / (1 + V/q_ * K + K * K);
                a0_ = (1 + 1/q_ * K + K * K) * norm;
                a1_ = 2 * (K * K - 1) * norm;
                a2_ = (1 - 1/q_ * K + K * K) * norm;
                b1_ = a1_;
                b2_ = (1 - V/q_ * K + K * K) * norm;
            }
            break;
        case filterType::LOWSHELF:
            if (peak_gain_ >= 0)
            {   // boost
                norm = 1 / (1 + sqrt(2) * K + K * K);
                a0_ = (1 + sqrt(2*V) * K + V * K * K) * norm;
                a1_ = 2 * (V * K * K - 1) * norm;
                a2_ = (1 - sqrt(2*V) * K + V * K * K) * norm;
                b1_ = 2 * (K * K - 1) * norm;
                b2_ = (1 - sqrt(2) * K + K * K) * norm;
            }
            else
            {   // cut
                norm = 1 / (1 + sqrt(2*V) * K + V * K * K);
                a0_ = (1 + sqrt(2) * K + K * K) * norm;
                a1_ = 2 * (K * K - 1) * norm;
                a2_ = (1 - sqrt(2) * K + K * K) * norm;
                b1_ = 2 * (V * K * K - 1) * norm;
                b2_ = (1 - sqrt(2*V) * K + V * K * K) * norm;
            }
            break;
        case filterType::HIGHSHELF:
            if (peak_gain_ >= 0)
            {   // boost
                norm = 1 / (1 + sqrt(2) * K + K * K);
                a0_ = (V + sqrt(2*V) * K + K * K) * norm;
                a1_ = 2 * (K * K - V) * norm;
                a2_ = (V - sqrt(2*V) * K + K * K) * norm;
                b1_ = 2 * (K * K - 1) * norm;
                b2_ = (1 - sqrt(2) * K + K * K) * norm;
            }
            else
            {   // cut
                norm = 1 / (V + sqrt(2*V) * K + K * K);
                a0_ = (1 + sqrt(2) * K + K * K) * norm;
                a1_ = 2 * (K * K - 1) * norm;
                a2_ = (1 - sqrt(2) * K + K * K) * norm;
                b1_ = 2 * (K * K - V) * norm;
                b2_ = (V - sqrt(2*V) * K + K * K) * norm;
            }
            break;
    }
    usleep(500);
    return;
}
