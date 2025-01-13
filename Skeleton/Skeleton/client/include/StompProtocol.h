#pragma once

#include "ConnectionHandler.h"
#include <string>
#include <map>
class StompProtocol
{
private:
    bool terminateKeyboard;          
    bool terminateServerResponses;
    ConnectionHandler &connection;  



public:
    StompProtocol(ConnectionHandler& connection);
    void server_response_process();
    vector<string> static split(string line, char delimiter);
    Event parseEventReport(string report);
};
