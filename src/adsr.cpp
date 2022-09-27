/**
 * @file adsr.cpp
 * @author Markus Wende and Robert Pelzer
 * @brief ADSR class implementation.
 */

#include "adsr.h"

ADSR::ADSR(void)
{
    Reset();
}

inline
float
ADSR::Process()
{
    if (state_ == noteState::ATTACK)
    {
        if (output_ == 0.0)
            output_ = 0.001;
        if (old_state_ >= 2)
            output_ = 0.001;
        // multiplicator goes from 0.04 to 0.00002
        // with release_time from 1 to 99
        output_ = output_ + output_ * (1 - (exp(-log10((1.0 + 10) / 10 ) / attack_time_) + 0.0004));
        old_state_ = state_;
        if (output_ >= 0.99) {
            output_ = 1.0;
            state_ = noteState::DECAY;
        }
    } else if (state_ == noteState::DECAY) {
        // multiplicator goes from 0.99924 to 0.999992
        // with release_time from 1 to 99
        output_ = output_ * (0.99 + (exp(-log10((2.0 + 10) / 10 ) / decay_time_) / 100));
        old_state_ = state_;
        if (output_ <= sustain_level_ / 100) {
            state_ = noteState::SUSTAIN;
        }
    } else if (state_ == noteState::SUSTAIN) {
        // sustain goes from 1 to 99
        output_ = sustain_level_ / 100;
    } else if (state_ == noteState::RELEASE) {
        // multiplicator goes from 0.99924 to 0.999992
        // with release_time from 1 to 99
        output_ = output_ * (0.99 + (exp(-log10((2.0 + 10) / 10 ) / release_time_) / 100));
        old_state_ = state_;
        if (output_ <= 0.001) {
            output_ = 0.0;
            state_ = noteState::NOTE_OFF;
        }
      
    } else {
        output_ = 0.0;
        state_ = noteState::NOTE_OFF;
    }

    return output_;
}

void
ADSR::Reset()
{
    state_ = noteState::NOTE_OFF;
    output_ = 0.0;

    attack_time_ = 1.0;
    decay_time_ = 1.0;
    sustain_level_ = 99;
    release_time_ = 1.0;
}