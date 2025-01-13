#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "ConnectionHandler.h"
#include "keyboardInput.h"
#include "InputStream.h"
#include "StompProtocol.h"


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

int main()
{
    std::string line;
    std::cout << "Enter commands (type 'exit_program' to stop):" << std::endl;

    // Create a ConnectionHandler object
    ConnectionHandler connection("", 0); // Temporary initialization; will be updated in Login
    InputStream inputStream();
    StompProtocol stompProtocol(connection);
    Login(connection);

    // Initialize InputStream and StompProtocol with connection
    

    // Create threads to run InputStream and StompProtocol tasks
    std::thread th1(std::ref(inputStream)); // Start `task1` in thread `th1`
    std::thread th2(std::ref(stompProtocol)); // Start `task2` in thread `th2`

    // Wait for threads to complete
    th1.join();
    th2.join();

    std::cout << "Exiting program." << std::endl;
    return 0;
}
