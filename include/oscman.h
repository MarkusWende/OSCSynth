#ifndef OSCMAN_H
#define OSCMAN_H

#include <lo/lo.h>
#include <iostream>
#include <vector>

#include "datatypes.h"

class OscMan {
private:
    // An object representing a thread containing an OSC server
    lo_server_thread st;

    // for each OSC message we store its path, value and type
    std::vector<double> messages;
    std::vector<std::string> paths;
    std::vector<std::string> types;

    // stored value vectors
    std::vector<dMess> dMessages;
    std::vector<iMess> iMessages;
    std::vector<sMess> sMessages;

    // callback function wich is processed every main loop cycle
    static int double_callback(const char *path, const char *types, lo_arg ** argv,
                            int argc, lo_message data, void *user_data);

public:
    // Constructor
    OscMan(int p);

    // Getters
    double getLastDouble();
    double getLastInt();
    double getLastChar();
    std::string getLastPath();
    std::string getLastType();

};

#endif
