#include "StompProtocol.h"
#include "../include/event.h"
#include <iostream>
#include <sstream>  // For stringstream operations
#include "keyboardInput.h"
#include <stdexcept>

using namespace std;


StompProtocol::StompProtocol(ConnectionHandler* conn): terminateKeyboard(false), terminateServerResponses(false),
connection(conn)
{
    
}

void StompProtocol::run()
{
    while (!terminateServerResponses) {
       
        std::string answer;
        bool hasAnswered = connection->getLine(answer); //Receiving a response from the server
        if (!hasAnswered) { //If the server connection was closed and no response receieved - close the client
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            terminateKeyboard = true;
            break;
        }
        if(hasAnswered) { //If a response from the server received
            StompFrame frame(answer);
            cout << answer;
            if (frame.getCommand() == "RECEIPT") {
                map<string, string> headers = frame.getHeaders();
                string receiptID = headers.find("receipt-id")->second;
                string receiptCOMMAND = connection->findReceiptCommand(receiptID);
                if (receiptCOMMAND == "DISCONNECT") { //If the receipt received is for a DISCONNECT frame - close the connection
                    cout << "bye bye" << endl;
                    terminateServerResponses = true;
                    connection->close();
                }
            }
            else if (frame.getCommand() == "ERROR") {
                terminateKeyboard = true;
                terminateServerResponses = true;
            }
            else if (frame.getCommand() == "MESSAGE") { //If a message was received - a user sent a report and it needed to be saved
                string report = frame.getBody();
                vector<string> lines = split_str(report,'\n');
                string userLine = lines[0];
                int index = userLine.find(':') + 1;
                string user = userLine.substr(index);
                Event object = parseEventReport(report);
                string channel_name = object.get_channel_name();
                connection->addReport(user, channel_name, object);
            }
        }
    }
}

vector<string> StompProtocol::split(string line, char delimiter)
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
                if (part.size() > 0)
                    parts.push_back(part);
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

Event StompProtocol::parseEventReport(string report) {
    vector<string> parts = StompProtocol::split(report, '\n');
    string user, channel_name, city, event_name, time, description;
    map<string, string> general_information;
    bool details = true, general = false;

    string previousLine;

    for (const string& line : parts) {
        if (details) {
            if (line.find("channel name:") != string::npos) {
                size_t index = line.find(':');
                channel_name = line.substr(index + 1);
            } else if (line.find("city:") != string::npos) {
                size_t index = line.find(':');
                city = line.substr(index + 1);
            } else if (line.find("event name:") != string::npos) {
                size_t index = line.find(':');
                event_name = line.substr(index + 1);
            } else if (line.find("time:") != string::npos) {
                size_t index = line.find(':');
                time = line.substr(index + 1);
            }
        }

        if (line == "general information:") {
            details = false;
        }

        if (general) {
            size_t index = line.find(':');
            if (index != string::npos) {
                string header = line.substr(0, index);
                string value = line.substr(index + 1);
                general_information[header] = value;
            }
        } else if (previousLine == "description:") {
            description = line;
        }

        if (line == "team a updates:") {
            general = false;
        }

        previousLine = line;
    }

    // Validate the required fields
    if (channel_name.empty() || city.empty() || event_name.empty() || time.empty() || description.empty()) {
        throw runtime_error("Missing required fields in the report.");
    }

    // Create and return the Event object
    int date_time = stoi(time);
    return Event(channel_name, city, event_name, date_time, description, general_information);
}

