/**
 * @file distortion.h
 * @author Markus Wende and Robert Pelzer
 * @brief Distortion class can be used to give the sound more color, by manipulation e.g. the gain.
 */

#pragma once

#include <iostream>
#include <cmath>

class Distortion {
public:

	// CONSTRUCTOR
    /**
     * @brief Standard Constructor.
     */
    Distortion();

    /**
     * @brief Constructor with parameters.
     * @param drive Drive as a double.
     */
    Distortion(double drive);

    // DESCTRUCTOR
    /**
     * @brief Standard Destructor.
     */
    ~Distortion();

    // SETTER
    /**
     * @brief Set the drive of the distortion.
     * @param drive Drive as a double.
     * @return Return void.
     */
    void SetDrive(double drive);

    /**
     * @brief Set the range of the distortion.
     * @param range Range as a double.
     * @return Return void.
     */
    void SetRange(double range);

    /**
     * @brief Set the blend of the distortion.
     * @param blend Blend as a double.
     * @return Return void.
     */
    void SetBlend(double blend);

    /**
     * @brief Process a value and return the distorted value.
     * @param in Input value as a double.
     * @return Return distorted value as a double.
     */
    double Process(double in);

private:
    double drive_;      /**< Drive of the distortion. */
    double range_;      /**< Range of the distortion. */
    double blend_;      /**< Blend of the distortion. */

};

/**
 * @brief Implementation of the Inline function Process.
 */
inline
double
Distortion::Process(double in)
{
    auto out = in;
    out = (((2.0 / M_PI) * std::atan(out) * blend_) + (in * (1.0 / blend_)));

    return out;
}
