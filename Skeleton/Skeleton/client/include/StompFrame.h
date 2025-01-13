#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "../include/ConnectionHandler.h"
#include "../include/event.h"

using namespace std;

class StompFrame
{
private:
	string STOMPCommand;
	map<string, string> headers;
	string body;
	vector<string> split(string line, char delimiter);
public:
	StompFrame(string frame);
	StompFrame(string command, map<string, string> headers, string body);
	string createFrame();
	string getCommand();
	map<string, string> getHeaders();
	string getBody();
};