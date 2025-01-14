
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
    : recieptId(0), currentClient("") {};

// Map to associate string commands with enum values
const std::unordered_map<std::string, Command> commandMap = {
    {"login", Command::LOGIN},
    {"logout",
     Command::LOGOUT},
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

void InputStream::run(ConnectionHandler connection)
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
            
            // Process commands using if-else instead of switch
            if (command == Command::JOIN)
            {
                if (input.size() < 2)
                {
                    std::cout << "Error: join requires a channel name." << std::endl;
                }
                else
                {
                    std::string channel_name = input[1];
                    std::string message_join = "SUBSCRIBE\n"
                                               "destination:" +
                                               channel_name + "\n"
                                                              "id:" +
                                               std::to_string(channels.size()) + "\n"
                                                                                 "reciept:" +
                                               std::to_string(recieptId);
                    // Send the subscription message
                    if (!connection.sendLine(message_join))
                    {
                        std::cerr << "Failed to send subscription message for channel:" << channel_name << std::endl;
                        continue;
                    }
                    else
                    {
                        IncreamentRecieptId();
                    }
                }
            }
            else if (command == Command::EXIT)
            {
                if (input.size() < 2)
                {
                    std::cout << "Error: exit requires a channel name." << std::endl;
                }
                else
                {
                    int x = 0;
                    // Code for EXIT command (commented in your original code)
                }
            }
            else if (command == Command::REPORT)
            {
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
                        std::string message_report = "SEND\n\n"
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
                            message_report += "\tactive: " + it->second + "\n";
                        }

                        // Check and append the "forces_arrival_at_scene" key
                        it = general_info.find("forces_arrival_at_scene");
                        if (it != general_info.end())
                        {
                            message_report += "\tforces_arrival_at_scene: " + it->second + "\n";
                        }
                        message_report += "description:\n" + e.get_description();
                        connection.sendLine(message_report);
                    }
                }
            }
            else if (command == Command::SUMMARY)
            {
                if (input.size() < 4)
                {
                    std::cout << "Error: summary requires channel name, user, and file path." << std::endl;
                }
                else
                {
                    std::string channel_name = input[1];
                    std::string user = input[2];
                    std::string file_path = input[3];
                    auto it = channels.find(channel_name);
                    if (it != channels.end())
                    {
                        Channel temp = it->second; // Assuming channels is a map with channel_name as the key

                        // Retrieve events by the user
                        std::vector<Event> events = temp.getEvents_ByUser(user);

                        // Now you can process the events or write them to a file
                        writeToFile(file_path, channel_name, events); // Assuming `writeToFile` is implemented as above
                    }
                    else
                    {
                        std::cerr << "Channel not found: " << channel_name << std::endl;
                    }
                }
            }
            else if (command == Command::LOGOUT)
            {
                std::string message_logout = "DISCONNECT\n"
                                             "reciept:" +
                                             std::to_string(recieptId) + "\n";
                if (connection.sendLine(message_logout))
                {
                    IncreamentRecieptId();
                }
            }
            else if (command == Command::UNKNOWN)
            {
                std::cout << "Unknown command: " << input[0] << std::endl;
            }
            else
            {
                std::cerr << "Unhandled case in commands!" << std::endl;
            }
        }
    }
}


void InputStream::IncreamentRecieptId()
{
    recieptId++;
}

void InputStream::writeToFile(const std::string &file_path, const std::string &channel_name, const std::vector<Event> &events)
{
    std::ofstream file(file_path);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return;
    }

    // Header
    file << "Channel < " << channel_name << " >\n";

    // Calculate statistics
    int total_reports = events.size();
    int active_count = 0;
    int forces_arrival_count = 0;

    for (Event event : events)
    {
        if (event.get_general_information().count("active") && event.get_general_information().at("active") == "true")
        {
            active_count++;
        }
        if (event.get_general_information().count("forces_arrival_at_scene") &&
            event.get_general_information().at("forces_arrival_at_scene") == "true")
        {
            forces_arrival_count++;
        }
    }

    // Write statistics

    file << "Stats:\n";
    file << "Total : " << total_reports << "\n";
    file << "active : " << active_count << "\n";
    file << "forces arrival at scene : " << forces_arrival_count << "\n";

    // Event Reports
    file << "Event Reports:\n";
    int report_number = 1;
    for (Event event : events)
    {
        file << "Report_" << report_number++ << ":\n";
        file << "\tcity : " << event.get_city() << "\n";
        file << "\tdate time : " << std::to_string(event.get_date_time()) << "\n";
        file << "\tevent name : " << event.get_name() << "\n";
        file << "\tsummary : " << event.get_description().substr(0, 100) << "...\n"; // Truncate description for summary
    }

    file.close();
    std::cout << "File written successfully to: " << file_path << std::endl;
}