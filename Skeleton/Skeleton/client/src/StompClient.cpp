#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "ConnectionHandler.h"
#include "keyboardInput.h"


ConnectionHandler* Login() {
    std::string line;
    ConnectionHandler* connection = nullptr;
    keyboardInput keyboard;
    while (connection == nullptr) {
        std::cout << "Enter login command (e.g., 'login hostname:port'):\n";
        std::getline(std::cin, line);

        std::vector<std::string> input = keyboard.split_str(line, ' ');

        if (input.size() > 1 && input[0] == "login") {
            try {
                // Split host and port
                std::vector<std::string> host_port = keyboard.split_str(input[1], ':');
                if (host_port.size() != 2) {
                    throw std::invalid_argument("Invalid format. Use hostname:port.");
                }

                // Convert port to short
                short port = static_cast<short>(std::stoi(host_port[1]));
                connection = new ConnectionHandler(host_port[0], port);

                // Attempt to connect
                if (connection->connect()) {
                    std::cout << "Login successful!\n";
                    return connection;
                } else {
                    std::cerr << "Failed to connect. Please try again.\n";
                    delete connection;
                    connection = nullptr; // Reset to null for retry
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << "\nPlease try again.\n";
            }
        } else {
            std::cerr << "Invalid command. Use 'login hostname:port'.\n";
        }
    }

    return nullptr;
}

int main() {
    std::string line;
    std::cout << "Enter commands (type 'exit_program' to stop):" << std::endl;
    ConnectionHandler* logged=Login();

    std::cout << "Exiting program." << std::endl;
    return 0;
}
