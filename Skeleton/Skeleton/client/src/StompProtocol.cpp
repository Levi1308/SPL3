#include "StompProtocol.h"
#include "../include/event.h"
#include <iostream>
#include <sstream>  // For stringstream operations
#include "keyboardInput.h"

StompProtocol::StompProtocol(ConnectionHandler& conn) : terminateKeyboard(false), terminateServerResponses(false),connection(conn)
{
    
}

void StompProtocol::run()
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
            StompFrame frame(answer);
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
                vector<string> lines = split_str(report,'\n');
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


