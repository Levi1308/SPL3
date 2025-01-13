
#include "InputStream.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "ConnectionHandler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <json.hpp>
#include "event.h"
#include "keyboardInput.h"
InputStream::InputStream()
    : departmentCounter(0), recieptId(0), currentClient("") {};

// Map to associate string commands with enum values
const std::unordered_map<std::string, Command> commandMap = {
    {"login", Command::LOGIN},
    {"join", Command::JOIN},
    {"exit", Command::EXIT},
    {"report", Command::REPORT},
    {"summary", Command::SUMMARY}};

// Function to convert string to Command enum
Command getCommand(const std::string &cmdStr)
{
    auto it = commandMap.find(cmdStr);
    if (it != commandMap.end())
    {
        return it->second;
    }
    return Command::UNKNOWN;
}

void InputStream::operator()(ConnectionHandler connection)
{
    std::string line;
    keyboardInput keyboard;
    while (true)
    {
        // Get a full line of input
        std::getline(std::cin, line);

        // Exit if the user types "exit_program"
        if (line == "exit_program")
        {
            break;
        }

        // Split the input into command and arguments
        std::vector<std::string> input = keyboard.split_str(line, ' ');
        if (!input.empty())
        {
            Command command = getCommand(input[0]); // Convert first word to enum
            // Process commands
            switch (command)
            {
            case Command::JOIN:
                if (input.size() < 2)
                {
                    std::cout << "Error: join requires a channel name." << std::endl;
                }
                else
                {
                    std::string channel_name = input[1];
                    std::string message = "SUBSCRIBE\n"
                                          "destination:" +
                                          channel_name + "\n"
                                                         "id:" +
                                          std::to_string(departmentCounter) + "\n"
                                                                              "reciept:" +
                                          std::to_string(recieptId)+"\0";
                    // Send the subscription message
                    if (!connection.sendLine(message))
                    {
                        std::cerr << "Failed to send subscription message for channel:" << channel_name << std::endl;
                        continue;
                    }
                    else
                    {
                        departments.insert(std::make_pair(channel_name, departmentCounter));
                        IncreamentDepartments();
                        IncreamentRecieptId();
                    }
                }
                break;

            case Command::EXIT:
                if (input.size() < 2)
                {
                    std::cout << "Error: exit requires a channel name." << std::endl;
                }
                else
                {
                    std::string channel_name = input[1];
                    int departmentId;
                    auto it = departments.find(channel_name); // Find the key
                    if (it != departments.end())
                    {
                        departmentId = it->second; // Access the value
                        std::cout << "Department ID: " << departmentId << std::endl;
                    }
                    else
                    {
                        std::cout << "Department not found!" << std::endl;
                        break;
                    }
                    std::string message = "UNSUBSCRIBE\n"
                                          "id:" +
                                          std::to_string(departmentId) + "\n"
                                                                         "reciept" +
                                          std::to_string(recieptId)+"\0";
                }
                break;

            case Command::REPORT:
                if (input.size() < 2)
                {
                    std::cout << "Error: report requires a file path." << std::endl;
                }
                else
                {
                    std::string file_path = input[1];
                    // Parse the events file
                    names_and_events parsedData = parseEventsFile(file_path);
                    // Extract channel name and events
                    std::string channelName = parsedData.channel_name;
                    std::vector<Event> events = parsedData.events;
                    for (Event e : events)
                    {
                        std::string message = "SEND\n\n"
                                              "user" +
                                              currentClient + "\n"
                                                              "city" +
                                              e.get_city() + "\n"
                                                             "event name:" +
                                              e.get_name() + "\n"
                                                             "date time:" +
                                              std::to_string(e.get_date_time()) + "\n"
                                                                                  "general information\n";
                        const auto &general_info = e.get_general_information();

                        // Check and append the "active" key
                        auto it = general_info.find("active");
                        if (it != general_info.end())
                        {
                            message += "\tactive: " + it->second + "\n";
                        }

                        // Check and append the "forces_arrival_at_scene" key
                        it = general_info.find("forces_arrival_at_scene");
                        if (it != general_info.end())
                        {
                            message += "\tforces_arrival_at_scene: " + it->second + "\n";
                        }
                        message+="description:\n"+e.get_description()+"\0";
                        
                    }
                }
                break;

            case Command::SUMMARY:
                if (input.size() < 4)
                {
                    std::cout << "Error: summary requires channel name, user, and file path." << std::endl;
                }
                else
                {
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
void InputStream::IncreamentDepartments()
{
    departmentCounter++;
}
void InputStream::IncreamentRecieptId()
{
    recieptId++;
}

