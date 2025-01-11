#pragma once

#include "ConnectionHandler.h"
#include <string>
class StompProtocol
{
private:
bool shouldTerminate = false;
//Connections<String> connections;
int messageID = 0; //Generator of message ID
int connectionID; //The connection ID of the ConnectionHandler that's currently using this protocol


    
public:
StompProtocol();
void start (int connectionId,ConnectionHandler connection);
void process(std::string message) ;
bool shouldTerminated();
};
