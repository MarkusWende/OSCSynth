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
//  The biquad filter class is the implementation of a digital biquad filter, 
//  which is a second-order recursive linear filter, containing two poles and two zeros. 
//  The source code was taken from the following website: https://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
//  Some changes were made to the filter to make it fit our needs. 
//  The Biquad has seven different selectable filters (lowpass, high pass, band pass, notch, peak, low shelf, high shelf). 
//  For each filter a set of formulas is implemented which calculate the filter coefficients. 
//  The advantage of using this biquad filter is, that it is a pretty flexible solution, 
//  however on the downside of is the fact that every little parameter change of the filter leads to a re-calculation of the coefficients. 
//  Fortunately, this doesn´t create any processing problems.
//  The lowpass, high pass, band pass and the peak filter respond to a Q value which determines bandwidth for the band pass filter, 
//  a steepness for low and high pass filters and the width of the peak filter. The low shelf, high shelf and peak filter don´t respond to Q value, but to a peak Gain. 
//  A high gain additionally led to distortion, which in some cases was very nice sounding. 
//  However, we undid this rise of gain to be able to implement a general distortion class. 
//  This had the effect that the peak gain didn´t increase the signals volume, but in praxis has a similar effect as Q for the other filters.

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
    // CONSTRUCTEUR
    /**
     * @brief Standard Constructor.
     */
    Biquad();

    /**
     * @brief Constructor with parameters.
     * @param type Type of the filter, like filterType::LOWPASS, filterType::PEAK etc.
     * @param fc Filter cut off frequency as a double.
     * @param q The Q value (quality factor) as a double.
     * @param peakGain Gain of the filter, for filterType::PEAK, filterType::LOWSHELF and filterType::HIGHSHELF.
     */
    Biquad(int type, double fc, double q, double peakGain);

    // DESCTRUCTOR
    ~Biquad();

    // SETTER
    /**
     * @brief Set the filter type. See \ref filterType
     * @param type Type of the filter, like filterType::LOWPASS, filterType::PEAK etc.
     * @return Return void.
     */
    void SetType(filterType type);

    /**
     * @brief Set the filter quality factor (Q) of the filter.
     * @param q The Q value (quality factor) as a double.
     * @return Return void.
     */
    void SetQ(double q);

    /**
     * @brief Set the filter cut off frequency of the filter.
     * @param fc The Q value (quality factor) as a double
     * @return Return void.
     */
    void SetFc(double fc);

    /**
     * @brief Set peak gain.
     * @param peakGain Gain of the filter, for filterType::PEAK, filterType::LOWSHELF and filterType::HIGHSHELF.
     * @return Return void.
     */
    void SetPeakGain(double peakGain);

    /**
     * @brief Set gain ruduction on / off.
     * @param status Gain reduction status as a bool. True = on and False = off.
     * @return Return void.
     */
    void SetGainReduce(bool status);

    /**
     * @brief Process a value and return the filtered value.
     * @param in Input value as a double.
     * @return Return Filtered value as a double.
     */
    double Process(double in);

    /**
     * @brief Print the filter type to the screen/file.
     * @return Return void.
     */
    void Status();
    
private:
    /**
     * @brief Set the filter quality factor (Q) of the filter
     * @param q The Q value (quality factor) as a double
     * @return Return void.
     */
    void calc_biquad();

    int     type_;                      /**< Type of the filter. */
    double  a0_, a1_, a2_, b1_, b2_;    /**< Filter coefficients. */
    double  fc_, q_, peak_gain_;        /**< Cut off frequency; q value; peak gain. */
    double  z1_, z2_;                   /**< Z delays. */
    bool    gain_reduce_;               /**< If a gain reduction is applied or not. */
};

/**
 * @brief Inline function to process a value.
 * @return Return double.
 */
inline double Biquad::Process(double in)
{
    auto out = in * a0_ + z1_;
    z1_ = in * a1_ + z2_ - b1_ * out;
    z2_ = in * a2_ - b2_ * out;
    
    // 	Section to reduce gain in areas where peak gain was applied earlier. 
    //	This leads to a reduction of the volume, however a high peakGain then works similar to the Q factor
    if (gain_reduce_)
    	out = out / pow(10, peak_gain_/20);
    
    return out;
}
