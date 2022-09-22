#ifndef DATATYPES_H
#define DATATYPES_H

#include <string>

/* struct for the osc double message
 * type: 'f' -> double (float), type of the osc message value
 */
struct dMess {
    std::string type;
    std::string path;
    double val;
};

/* struct for the osc integer message
 * type: 'i' -> integer, type of the osc message value
 */
struct iMess {
    std::string type;
    std::string path;
    int val;
};

/* struct for the osc char message
 * type: 's' -> char, type of the osc message value
 */
struct sMess {
    std::string type;
    std::string path;
    char val;
};

/* struct for the midi message
 * byte1 = status byte
 * byte2 = data byte 1
 * byte3 = data byte 2
 * stamp = time stamp
 * hasBeenProcessed = whether the midi event is processed or not
 */
struct midiMessage {
	int byte1;
	int byte2;
	double byte3;
	double stamp;
	bool hasBeenProcessed;

};

#endif // DATATYPES_H
