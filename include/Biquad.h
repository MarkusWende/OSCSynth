/**
 * @file adsr.h
 * @author Markus Wende and Robert Pelzer
 * @brief Biquad class represents the filter element of a synthesizer, which the sound over frequency.
 */

//  Created by Nigel Redmon on 11/24/12
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the Biquad code:
//  https://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
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

#pragma once

#include <math.h>

// Filter types
enum filterType
{
    LOWPASS = 0,                /**< The filter is a lowpass. */
    HIGHPASS = 1,               /**< The filter is a highpass. */
    BANDPASS = 2,               /**< The filter is a bandpass. */
    NOTCH = 3,                  /**< The filter is a notch filter. */
    PEAK = 4,                   /**< The filter is a peak filter. */
    LOWSHELF = 5,               /**< The filter is a lowshelf. */
    HIGHSHELF = 6               /**< The filter is a highschelf. */
};

class Biquad
{
public:
    // CONSTRUCTEUR & DESCTRUCTOR
    Biquad();
    /**
     * @brief Constructor with parameters
     * @param type Type of the filter, like filterType::LOWPASS, filterType::PEAK etc.
     * @param fc Type of the filter, like filterType::LOWPASS, filterType::PEAK etc.
     * @param q Type of the filter, like filterType::LOWPASS, filterType::PEAK etc.
     * @param peakGain Type of the filter, like filterType::LOWPASS, filterType::PEAK etc.
     */
    Biquad(int type, double fc, double q, double peakGain);
    ~Biquad();

    // Setter
    /**
     * @brief Set the filter type. See \ref filterType
     * @param type Type of the filter, like filterType::LOWPASS, filterType::PEAK etc.
     * @return Return void.
     */
    void    SetType(filterType type);

    /**
     * @brief Set the filter quality factor (Q) of the filter
     * @param q The Q value (quality factor) as a double
     * @return Return void.
     */
    void    SetQ(double q);

    /**
     * @brief Set the filter quality factor (Q) of the filter
     * @param q The Q value (quality factor) as a double
     * @return Return void.
     */
    void    SetFc(double fc);

    /**
     * @brief Set the filter quality factor (Q) of the filter
     * @param q The Q value (quality factor) as a double
     * @return Return void.
     */
    void    SetPeakGain(double peakGain);

    /**
     * @brief Set the filter quality factor (Q) of the filter
     * @param q The Q value (quality factor) as a double
     * @return Return void.
     */
    void    SetGainReduce(int on);

    /**
     * @brief Set the filter quality factor (Q) of the filter
     * @param q The Q value (quality factor) as a double
     * @return Return void.
     */
    void    SetBiquad(double peakGain);

    /**
     * @brief Set the filter quality factor (Q) of the filter
     * @param q The Q value (quality factor) as a double
     * @return Return void.
     */
    float   Process(float in);

    /**
     * @brief Set the filter quality factor (Q) of the filter
     * @param q The Q value (quality factor) as a double
     * @return Return void.
     */
    void    Status();
    
private:
    /**
     * @brief Set the filter quality factor (Q) of the filter
     * @param q The Q value (quality factor) as a double
     * @return Return void.
     */
    void calc_biquad();

    int     type_;                      /**< Type of the filter. */
    double  a0_, a1_, a2_, b1_, b2_;    /**< The filter is a lowpass. */
    double  fc_, q_, peak_gain_;        /**< The filter is a lowpass. */
    double  z1_, z2_;                   /**< The filter is a lowpass. */
    bool    gain_reduce_;               /**< The filter is a lowpass. */
};

/**
 * @brief Reset or init the ADSR.
 * @return Return void.
 */
inline float Biquad::Process(float in)
{
    double out = in * a0_ + z1_;
    z1_ = in * a1_ + z2_ - b1_ * out;
    z2_ = in * a2_ - b2_ * out;
    
    // 	Section to reduce gain in areas where peak gain was applied earlier. 
    //	This leads to a reduction of the volume, however a high peakGain then works similar to the Q factor
    if (gain_reduce_)
    	out = out / pow(10, peak_gain_/20);
    
    return out;
}
