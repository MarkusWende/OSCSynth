#pragma once

#include <string>

/**
 * @brief Struct for osc double messages.
 */
struct dMess
{
    std::string type;       /**< Message value type. */
    std::string path;       /**< Message path / command. */
    double val;             /**< Message data. */
};

/**
 * @brief Struct for osc integer messages.
 */
struct iMess
{
    std::string type;       /**< Message value type. */
    std::string path;       /**< Message path / command. */
    int val;                /**< Message data. */
};

/**
 * @brief Struct for osc char messages.
 */
struct sMess
{
    std::string type;       /**< Message value type. */
    std::string path;       /**< Message path / command. */
    char val;               /**< Message data. */
};

/**
 * @brief Struct for midi messages.
 */
struct midiMessage
{
	int byte1;              /**< Status byte. */
	int byte2;              /**< Data byte 1. */
	double byte3;           /**< Data byte 2. */
	double stamp;           /**< Time stamp. */
	bool hasBeenProcessed;  /**< Whether the midi event is processed or not. */
};
