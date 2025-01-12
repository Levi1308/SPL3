
#include "InputStream.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "ConnectionHandler.h"

InputStream::InputStream():subscribeCounter(0){};


// Map to associate string commands with enum values
const std::unordered_map<std::string, Command> commandMap = {
    {"login", Command::LOGIN},
    {"join", Command::JOIN},
    {"exit", Command::EXIT},
    {"report", Command::REPORT},
    {"summary", Command::SUMMARY}
};

// Function to convert string to Command enum
Command getCommand(const std::string& cmdStr) {
    auto it = commandMap.find(cmdStr);
    if (it != commandMap.end()) {
        return it->second;
    }
    return Command::UNKNOWN;
}
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
void InputStream::operator()(ConnectionHandler connection){
    std::string line;
    while (true) {
        // Get a full line of input
        std::getline(std::cin, line);

        // Exit if the user types "exit_program"
        if (line == "exit_program") {
            break;
        }

        // Split the input into command and arguments
        std::vector<std::string> input = split(line, ' ');
        if (!input.empty()) {
            Command command = getCommand(input[0]); // Convert first word to enum
            // Process commands
            switch (command) {
                case Command::JOIN:
                    if (input.size() < 2) {
                        std::cout << "Error: join requires a channel name." << std::endl;
                    } else {
                        std::string channel_name = input[1];
                        std::string message = "SUBSCRIBE\n"
                          "destination:" + channel_name + "\n"
                          "id:" + std::to_string(subscribeCounter);

                        // Send the subscription message
                        if (!connection.sendLine(message)) {
                            std::cerr << "Failed to send subscription message for channel: " << channel_name << std::endl;
                            continue;
                        }
                    }
                    break;

                case Command::EXIT:
                    if (input.size() < 2) {
                        std::cout << "Error: exit requires a channel name." << std::endl;
                    } else {
                        std::string channel_name = input[1];
                        std::cout << "Exiting channel: " << channel_name << std::endl;
                    }
                    break;

                case Command::REPORT:
                    if (input.size() < 2) {
                        std::cout << "Error: report requires a file path." << std::endl;
                    } else {
                        std::string file_path = input[1];
                        std::cout << "Generating report for file: " << file_path << std::endl;
                    }
                    break;

                case Command::SUMMARY:
                    if (input.size() < 4) {
                        std::cout << "Error: summary requires channel name, user, and file path." << std::endl;
                    } else {
                        std::string channel_name = input[1];
                        std::string user = input[2];
                        std::string file_path = input[3];
                        std::cout << "Generating summary for channel: " << channel_name
                                  << ", user: " << user
                                  << ", file: " << file_path << std::endl;
                    }
                    break;

                case Command::UNKNOWN:
                default:
                    std::cout << "Unknown command: " << input[0] << std::endl;
                    break;
            }
        }
    }
    
}
void InputStream::IncreamentSubscribers(){
        subscribeCounter++;
}