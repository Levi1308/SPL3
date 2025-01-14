#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "../include/ConnectionHandler.h"
#include "../include/event.h"

using namespace std;

class Channel{
private:
    string channel_name;
    map<string, vector<Event>> events;
     

public:
    Channel(string channel_name);
    virtual ~Channel();
    void addEvent(string user, Event event);
	vector<Event> getEvents_ByUser(string user);
	string get_name();
    int getNumberDepartment();
};