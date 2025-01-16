#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "ConnectionHandler.h"
#include "keyboardInput.h"
#include "InputStream.h"
#include "StompProtocol.h"
#include <stdlib.h>
#include <thread>

int main(int argc, char *argv[])
{
    std::string line;
    bool flag = false;
    while (!flag)
    {
        std::getline(std::cin, line);
        std::vector<std::string> input = split_str(line, ' ');
        if (input.size() == 4 && input[0] == "login")
        {
            try
            {
                // Split host and port
                std::vector<std::string> host_port = split_str(input[1], ':');
                if (host_port.size() == 2)
                {
                    // Convert port to short
                    short port = static_cast<short>(std::stoi(host_port[1]));
                    // Update the existing `ConnectionHandler` instance
                    ConnectionHandler* connection=new ConnectionHandler(host_port[0], port);

                    // Attempt to connect
                    if (connection->connect())
                    {
                        connection->connectUser(input[2]);
                        std::string message_login="CONNECT\n";
                        message_login+="accept-version :1.2\n";
                        message_login+="host:stomp.cs.bgu.ac.il\n";
                        message_login+="login:"+input[2]+"\n";
                        message_login+="passcode:"+input[3]+"\n";
                        if(connection->sendFrame(message_login)){
                        std::cout << "Login successful!\n";
                        try
                        {
                            StompProtocol protocol(connection);
                            InputStream inputStream;
                            std::cout << std::to_string(connection->isLogined())<< std::endl;
                            inputStream.run(connection);
                            std::thread serverThread(&StompProtocol::run, &protocol);
                            serverThread.join();
                            delete connection;
                        }
                        catch (const std::exception &e)
                        {
                            cout << "An error received, disconnecting.." << endl;
                        }
                        flag = true; // Exit loop on successful login
                    }
                    }
                    else
                    {
                        std::cerr << "Failed to connect. Please try again.\n";
                    }
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error: " << e.what() << "\nPlease try again.\n";
            }
        }
        else
        {
            std::cerr << "Invalid command. Use 'login hostname:port'.\n";
        }
    }

    return 0;
}