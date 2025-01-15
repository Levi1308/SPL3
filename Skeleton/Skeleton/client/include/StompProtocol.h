#pragma once

#include "ConnectionHandler.h"
#include "StompFrame.h"
#include <string>
#include <map>
#include <fstream>
#include <iostream>

class Event;

class StompProtocol
{
private:
    bool terminateKeyboard;          
    bool terminateServerResponses;
    ConnectionHandler &connection;  

public:
    StompProtocol(ConnectionHandler& conn);
    void run();
    Event parseEventReport(string report);
    vector<string> static split(string line, char delimiter);
};
