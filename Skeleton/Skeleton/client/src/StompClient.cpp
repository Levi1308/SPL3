#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "ConnectionHandler.h"
#include "keyboardInput.h"
#include "StompProtocol.h"
#include <stdlib.h>
#include <thread>

int main(int argc, char *argv[])
{
    std::cerr << "Enter client" << std::endl;
    while (true)
    {
        const short bufsize = 1024;
        char buf[bufsize];
        /* Receiving the first command - before creating the connection. If the connection is a valid login command - create the connection & the threads of the keyboard and server responses */
        std::cin.getline(buf, bufsize);
        std::string command(buf);
        vector<string> partsOfCommand = StompProtocol::split(command, ' ');
        if (partsOfCommand[0] == "login")
        {
            cout << "" << endl;
            if (partsOfCommand.size() == 4)
            {
                string host = partsOfCommand[1].substr(0, partsOfCommand[1].find(':'));
                short port = stoi(partsOfCommand[1].substr(partsOfCommand[1].find(':') + 1));
                ConnectionHandler connectionHandler(host,port);
                if (!connectionHandler.connect())
                {
                    std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
                }
                else
                {
                    string command = "CONNECT";
                    map<string, string> headers;
                    headers.insert({"accept-version", "1.2"});
                    headers.insert({"host", "stomp.cs.bgu.ac.il"});
                    headers.insert({"login", partsOfCommand[2]});
                    headers.insert({"passcode", partsOfCommand[3]});
                    StompFrame frame(command, headers, "");
                    string output = frame.createFrame();
                    cout<<output<<endl;
                    connectionHandler.connectUser(partsOfCommand[2]);
                    if (connectionHandler.sendFrame(output))
                    {
                        try
                        {

                            StompProtocol protocol(connectionHandler);
                            thread serverThread(&StompProtocol::server_response_process, &protocol); //This thread is responsible to receive answers from the server & process them
                            thread keyboardThread(&StompProtocol::run_keyboard, &protocol); //This thread is responsible to receive inputs from the keyboard, process them & send them to the server
                            serverThread.join();
                            keyboardThread.join();
                        }       
                        catch (const std::exception &e)
                        {
                            cout << "An error received, disconnecting.." << endl;
                        }
                    }
                    else
                    {
                        cout << "Could not loggin try again" << endl;
                    }
                }
            }
            else
            {
                cout << "login command needs 3 arguments: {port} {user} {passcode}" << endl;
            }
        }
    }
    return 0;
}
