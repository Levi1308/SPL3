#pragma once

#include "ConnectionHandler.h"
#include "StompFrame.h"
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include "Channel.h"
#include "keyboardInput.h"

// Enum for commands
enum class Command {
    LOGIN,
    JOIN,
    EXIT,
    REPORT,
    SUMMARY,
    UNKNOWN,
    LOGOUT
};
class Event;
using namespace std;

class StompProtocol
{
private:         
    bool terminateKeyboard;
    bool terminateServerResponses;
    ConnectionHandler &connection;  
    std::map<std::string,Channel> channels;
    int subscriptionId;
    std::map<std::string,int> subscriptions;
    bool loggedIn;
    std::condition_variable cv;
    std::mutex cvMutex;


public:
    StompProtocol(ConnectionHandler& conn);
    void run();
    Event parseEventReport(string report);
    vector<string> static split(string line, char delimiter);
    void run_keyboard();
    void IncreamentSubId();
    void writeToFile(const std::string &file_path, const std::string &channel_name, const std::vector<Event> &events);
};
