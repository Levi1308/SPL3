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
    ConnectionHandler *connection;  
    int subscriptionId;
    std::map<std::string,int> subscriptions;
    bool loggedIn;
    std::condition_variable cv;
    std::mutex cvMutex;
    queue<string> waitframe;

public:
    StompProtocol(ConnectionHandler* conn);
    void runServerInput();
    void processServerInput(string answer);
    Event parseEventReport(string report,string channelName);
    vector<string> static split(string line, char delimiter);
    void runkeyboardInput();
    void IncreamentSubId();
    void writeToFile(const std::string &file_path, const std::string &channel_name, std::vector<Event> events);
};
