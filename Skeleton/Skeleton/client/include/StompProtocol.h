#pragma once

#include "ConnectionHandler.h"
#include <string>
#include <map>
class StompProtocol
{
private:
    bool shouldTerminate;          // Will determine if the protocol should terminate
    ConnectionHandler connection;  // The connection handler for the current protocol
    int messageID;                 // Generator of message ID
    int connectionID;              // The connection ID of the ConnectionHandler

public:
    // Constructor: Initialize shouldTerminate to false
    StompProtocol();

    // Start method to initialize connectionID and connection
    void start(int connectionId, ConnectionHandler connection);

    // Method to process the incoming message
    void process(std::string message);

    // Method to check if the protocol should terminate
    bool connectionShouldTerminate();

    std::map<std::string, std::string> StompProtocol::parseStompMessage(const std::vector<std::string>& lines);
};
