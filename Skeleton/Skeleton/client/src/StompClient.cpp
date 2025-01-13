#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "ConnectionHandler.h"
#include <fstream>  // For std::ifstream


// Function to split a string by a delimiter
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(str);

    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token); // Add each token to the vector
    }
    return tokens;
}


ConnectionHandler* Login() {
    std::string line;
    ConnectionHandler* connection = nullptr;

    while (connection == nullptr) {
        std::cout << "Enter login command (e.g., 'login hostname:port'):\n";
        std::getline(std::cin, line);

        std::vector<std::string> input = split(line, ' ');

        if (input.size() > 1 && input[0] == "login") {
            try {
                // Split host and port
                std::vector<std::string> host_port = split(input[1], ':');
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


namespace Auxiliary {
    // Function to split a line into arguments (example implementation)
    std::vector<std::string> parseArguments(const std::string& line) {
        std::vector<std::string> args;
        std::istringstream iss(line);
        std::string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }
        return args;
    }
}

void FileReaderJson(const std::string& configFilePath) {
    try {
        // Open the config file
        std::ifstream configFile(configFilePath);
        if (!configFile.is_open()) {
            throw std::runtime_error("Failed to open config file: " + configFilePath);
        }
        std::string line;
    while (std::getline(configFile, line)) {
        std::vector<std::string> args = Auxiliary::parseArguments(line);
        if (args.empty() || args[0][0] == '#') {
            continue; 
        }
        std::cout<<line<<std::endl;
        
    }
    configFile.close();
    }
    catch (const std::exception& e) {
        // Handle any exceptions that occur during file I/O or JSON parsing
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    std::string line;
    std::cout << "Enter commands (type 'exit_program' to stop):" << std::endl;
    FileReaderJson("Skeleton/Skeleton/client/data/events1.json");
    //ConnectionHandler* logged=Login();

    std::cout << "Exiting program." << std::endl;
    return 0;
}
