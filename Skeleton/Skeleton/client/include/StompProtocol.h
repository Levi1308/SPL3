#pragma once

#include "ConnectionHandler.h"
#include "StompFrame.h"
#include <string>
#include <map>
#include <queue>
#include <fstream>
#include <iostream>
#include "Channel.h"
#include "keyboardInput.h"


class Event;
using namespace std;

class StompProtocol
{
private:         
    bool terminateKeyboard;
    bool terminateServerResponses;
    ConnectionHandler &connection;  
    int subscriptionId;
    std::map<std::string,int> subscriptions;
    

public:
    StompProtocol(ConnectionHandler& conn);
    void server_response_process();
    Event parseEventReport(string report,string channelName);
    vector<string> static split(string line, char delimiter);
    void run_keyboard();
    void IncreamentSubId();
    void writeToFile(const std::string &file_path, const std::string &channel_name, std::vector<Event> events);
};
