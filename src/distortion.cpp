/**
 * @file distortion.cpp
 * @author Markus Wende and Robert Pelzer
 * @brief Distortion class implementation.
 */

#include "distortion.h"

Distortion::Distortion()
{
    drive_ = 1.0;
    range_ = 0.8;
    blend_ = 0.8;
}

Distortion::Distortion(double drive)
{
    drive_ = drive;
}

Distortion::~Distortion()
{
}

void
Distortion::SetDrive(double drive)
{
    drive_ = drive;
}

void
Distortion::SetRange(double range)
{
    range_ = range;
}

void
Distortion::SetBlend(double blend)
{
    blend_ = blend;
}