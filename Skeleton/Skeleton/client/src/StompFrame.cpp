#include "StompFrame.h"

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>

vector<string> StompFrame::split(string line, char delimiter)
{
    vector<string> parts;
    while (line.size() > 0)
    {
        string part = "";
        std::size_t i = 0;
        for (i = 0; i < line.size(); i++)
        {
            if (delimiter == line[i])
            {
                if (part.size() > 0) {
                    parts.push_back(part);
                    parts.push_back("\n");
                    if((i != line.length() - 1) && delimiter == line[i+1]) {
                            parts.push_back("\n");
                    }
                }
                break;
            }
            else
                part += line[i];
        }
        if (i == line.size())
        {
            parts.push_back(part);
            line = "";
        }
        else
            line = line.substr(i + 1);
    }
    return parts;

}

StompFrame::StompFrame(string frame): STOMPCommand(), headers(), body()
{
    vector<string> frameLines = split(frame, '\n');
    STOMPCommand = frameLines.at(0);
    bool hasBody = false;
    int bodyIndex = 0;
    for (std::size_t i = 1; i < frameLines.size(); i++) {
        if (frameLines.at(i) == "\n" && frameLines.at(i+1) == "\n" && frameLines.at(i+2) != "\0") {
            hasBody = true;
            bodyIndex = i + 2;
            break;
        }
        if (frameLines.at(i) != "\n" && frameLines.at(i) != "\0") {
            string line = frameLines.at(i);
            int index = line.find(':');
            string header = line.substr(0, index);
            string value = line.substr(index + 1);
            headers.insert({ header, value });
        }
    }
    if (hasBody) {
        string bodyString = "";
        for (std::size_t i = bodyIndex; i < frameLines.size(); i++) {
            if (frameLines.at(i) != "\0") {
                bodyString.append(frameLines.at(i));
            }
        }
        body = bodyString;
    }
}

StompFrame::StompFrame(string command, map<string, string> headers, string body): STOMPCommand(command), headers(headers), body(body)
{
}

string StompFrame::createFrame()
{
    string frame = "";
    frame.append(STOMPCommand);
    frame.append("\n");
    for (auto& line : headers) {
        frame.append(line.first);
        frame.append(":");
        frame.append(line.second);
        frame.append("\n");
    }
    frame.append("\n\n");
    if (!body.empty()) {
        frame.append(body);
        frame.append("\n");
    }
    return frame;
}

string StompFrame::getCommand()
{
    return STOMPCommand;
}

map<string, string> StompFrame::getHeaders()
{
    return headers;
}

string StompFrame::getBody()
{
    return body;
}


