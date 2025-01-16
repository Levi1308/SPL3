<<<<<<< HEAD
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


/*
void Login(ConnectionHandler &connection)
{
    std::string line;
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
*/
int main (int argc, char *argv[])
{
    const short bufsize = 1024;
    char buf[bufsize];
    /* Receiving the first command - before creating the connection. If the connection is a valid login command - create the connection & the threads of the keyboard and server responses */
    std::cin.getline(buf, bufsize);
    std::string command(buf);
    vector<string> partsOfCommand = StompProtocol::split(command, ' ');
    if (partsOfCommand[0] == "login") {
        cout << "" << endl;
        if (partsOfCommand.size() == 4) {
            
            string host = partsOfCommand[1].substr(0, partsOfCommand[1].find(':'));
            short port = stoi(partsOfCommand[1].substr(partsOfCommand[1].find(':') + 1));
            ConnectionHandler connectionHandler(host, port);
            if (!connectionHandler.connect()) {
                std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
                return 1;
            }
            string command = "CONNECT";
            map<string, string> headers;
            headers.insert({ "accept-version", "1.2" });
            headers.insert({ "host", "stomp.cs.bgu.ac.il" });
            headers.insert({ "login", partsOfCommand[2] });
            headers.insert({ "passcode", partsOfCommand[3] });
            StompFrame frame(command, headers, "");
            string output = frame.createFrame();
            connectionHandler.connectUser(partsOfCommand[2]);
            connectionHandler.sendFrame(output);

            try{
            
              StompProtocol protocol(connectionHandler);
              InputStream inputStream; 
               // Start threads for input and server handling
               std::thread keyboardThread(&InputStream::run, &inputStream, std::ref(connectionHandler));
               std::thread serverThread(&StompProtocol::run, &protocol);

              // Join threads
               keyboardThread.join();
               serverThread.join();
        
            }
              catch(exception) {
                cout << "An error received, disconnecting.." << endl;
            }
      
        }
    }
    return 0;
}
=======
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
>>>>>>> 40246fb9df285409d32cbae5a8d326615d67a52b
