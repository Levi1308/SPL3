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



void Login(ConnectionHandler &connection)
{
    std::string line;
    keyboardInput keyboard;
    bool flag = false;

    while (!flag)
    {
        std::cout << "Enter login command (e.g., 'login hostname:port'):\n";
        std::getline(std::cin, line);

        std::vector<std::string> input = keyboard.split_str(line, ' ');

        if (input.size() > 1 && input[0] == "login")
        {
            try
            {
                // Split host and port
                std::vector<std::string> host_port = keyboard.split_str(input[1], ':');
                if (host_port.size() != 2)
                {
                    throw std::invalid_argument("Invalid format. Use hostname:port.");
                }

                // Convert port to short
                short port = static_cast<short>(std::stoi(host_port[1]));

                // Update the existing `ConnectionHandler` instance
                connection = ConnectionHandler(host_port[0], port);

                // Attempt to connect
                if (connection.connect())
                {
                    std::cout << "Login successful!\n";
                    flag = true; // Exit loop on successful login
                }
                else
                {
                    std::cerr << "Failed to connect. Please try again.\n";
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
}

int main (int argc, char *argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    std::string host = argv[1];
    short port = atoi(argv[2]);
    
    
    ConnectionHandler connectionHandler(host, port);
    

    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }

    if(1)
    {
    StompProtocol protocol(connectionHandler);
    InputStream inputStream; 
    std::thread keyboardThread(&InputStream::run, &inputStream, std::ref(connectionHandler));
    thread serverThread(&StompProtocol::operator(), &protocol);

    serverThread.join();
    keyboardThread.join();
    
    }
    std::cout << "Exiting program." << std::endl;
    return 0;
}
