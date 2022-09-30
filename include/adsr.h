/**
 * @file adsr.h
 * @author Markus Wende and Robert Pelzer
 * @brief ADSR class represents the envelope of a synthesizer, which shapes the sound over time.
 */

#pragma once

#include <math.h>

// states of the ADSR
enum noteState
{
    NOTE_OFF = 0,                       /**< ADSR is off. */
    ATTACK = 1,                         /**< ADSR is in attack state. */
    DECAY = 2,                          /**< ADSR is in decay state. */
    SUSTAIN = 3,                        /**< ADSR is in sustain state. */
    RELEASE = 4                         /**< ADSR is in release state. */
};

class ADSR
{
private:

    int     state_;                     /**< State of the ADSR. */
    int     old_state_;                 /**< Old state of the ADSR. */
    float   output_;                    /**< Output value, which the signal is multiplied with. */

    float   attack_time_;               /**< Attack time. */
    float   decay_time_;                /**< Decay time. */
    float   sustain_level_;             /**< Sustain level. */
    float   release_time_;              /**< Release time. */
    
public:
    // CONSTRUCTEUR & DESCTRUCTOR
	ADSR();
    ~ADSR();

    /**
     * @brief   Inline process function, which returns the value which the signal is muliplied with.
     *          The calculation depends on the envelope state and the following attributes:
     *              attack_time_
     *              decay_time_
     *              sustain_level_
     *              release_time_
     * @return Return the output as a float.
     */
	float Process();

    /**
     * @brief Reset or init the ADSR.
     * @return Return void.
     */
    void Reset();

    // GETTERS
    /**
     * @brief Set the attack time.
     * @return Return the output as a float.
     */
    float   GetOutput()     { return output_; };

    /**
     * @brief Set the attack time.
     * @return Return the state of the ADSR as an int.
     */
    int     GetState()      { return state_; };

    // SETTERS
    /**
     * @brief Set the attack time.
     * @param t attack time, as a float, range from 1.0f to 99.0f.
     * @return Return void.
     */
    void    SetAttack(float t)      { attack_time_ = t; };

    /**
     * @brief Set the attack time.
     * @param t decay time, as a float, range from 1.0f to 99.0f.
     * @return Return void.
     */
    void    SetDecay(float t)       { decay_time_ = t; };

    /**
     * @brief Set the attack time.
     * @param t release time, as a float, range from 1.0f to 99.0f.
     * @return Return void.
     */
    void    SetRelease(float t)     { release_time_ = t; };

    /**
     * @brief Set the sustain level.
     * @param level sustain level, as a float, range from 1.0f to 99.0f.
     * @return Return void.
     */
    void    SetSustain(float level) { sustain_level_ = level; };

    /**
     * @brief Set the state. See \ref noteState
     * @param state The state of the ADSR, e.g. can be noteState::NOTE_OFF, noteState::ATTACK etc.
     * @return Return void.
     */
    void    SetState(int state)     { state_ = state; };

};
