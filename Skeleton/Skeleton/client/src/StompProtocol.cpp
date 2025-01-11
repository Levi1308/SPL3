#include "StompProtocol.h"
#include <iostream>
#include <sstream>  // For stringstream operations

// Constructor to initialize member variables
StompProtocol::StompProtocol() : shouldTerminate(false), messageID(0), connectionID(0),connection("example",(short)1) {

}

// Start method to set connectionID and initialize connection
void StompProtocol::start(int connectionId, ConnectionHandler connection) {
    connectionID = connectionId;   // Set the connection ID
    this->connection = connection;  // Set the connection handler
    shouldTerminate = false;        // Reset termination flag on start
}


void StompProtocol::process(std::string message) {
   
}

// Method to check if the protocol should terminate
bool StompProtocol::connectionShouldTerminate() {
    return shouldTerminate;  // Return the termination status
}


std::map<std::string, std::string> StompProtocol::parseStompMessage(const std::vector<std::string>& lines) {
    // Initialize a map to store headers
    std::map<std::string, std::string> headers;

    // Loop through lines (skip the first line as it's the command)
    for (size_t i = 1; i < lines.size() && !lines[i].empty(); ++i) {
        std::string line = lines[i];
        // Trim whitespace from the line
        line.erase(0, line.find_first_not_of(" \t\n\r"));  // Remove leading whitespace
        line.erase(line.find_last_not_of(" \t\n\r") + 1);  // Remove trailing whitespace

        // Split the line at the first colon (":")
        size_t pos = line.find(":");
        if (pos != std::string::npos) {
            std::string headerName = line.substr(0, pos);
            std::string headerValue = line.substr(pos + 1);

            // Trim headerName and headerValue
            headerName.erase(headerName.find_last_not_of(" \t\n\r") + 1);  // Remove trailing whitespace
            headerValue.erase(headerValue.find_first_not_of(" \t\n\r"));  // Remove leading whitespace

            // Insert into the map
            headers[headerName] = headerValue;
        }
    }

    return headers;
}