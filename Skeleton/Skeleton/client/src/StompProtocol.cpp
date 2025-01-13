#include "StompProtocol.h"
#include "../include/event.h"
#include <iostream>
#include <sstream>  // For stringstream operations


StompProtocol::StompProtocol(ConnectionHandler& connection) : connection(connection), terminateKeyboard(false), terminateServerResponses(false)
{
}

void StompProtocol::server_response_process()
{
    while (!terminateServerResponses) {
        std::string answer;
        bool hasAnswered = connection.getLine(answer); //Receiving a response from the server
        if (!hasAnswered) { //If the server connection was closed and no response receieved - close the client
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            terminateKeyboard = true;
            break;
        }
        if(hasAnswered) { //If a response from the server received
           // StompFrame frame(answer);
            cout << answer;

            if (frame.getCommand() == "RECEIPT") {
                map<string, string> headers = frame.getHeaders();
                string receiptID = headers.find("receipt-id")->second;
                string receiptCOMMAND = connection.findReceiptCommand(receiptID);
                if (receiptCOMMAND == "DISCONNECT") { //If the receipt received is for a DISCONNECT frame - close the connection
                    cout << "bye bye" << endl;
                    terminateServerResponses = true;
                    connection.close();
                }
            }
            else if (frame.getCommand() == "ERROR") {
                terminateKeyboard = true;
                terminateServerResponses = true;
            }
            else if (frame.getCommand() == "MESSAGE") { //If a message was received - a user sent a report and it needed to be saved
                string report = frame.getBody();
                vector<string> lines = split(report, '\n');
                string userLine = lines[0];
                int index = userLine.find(':') + 1;
                string user = userLine.substr(index);
                Event object = parseEventReport(report);
                string channel_name = object.get_channel_name();
                connection.addReport(user, channel_name, object);
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
