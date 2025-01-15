

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
#include "InputStream.h"



InputStream::InputStream()
    : recieptId(0),channels(), subscriptionId(0),subscriptions(),terminateKeyboard(false) {};

// Map to associate string commands with enum values
const std::unordered_map<std::string, Command> commandMap = {
    {"login", Command::LOGIN},
    {"logout",Command::LOGOUT},
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

void InputStream::run(ConnectionHandler& connection)
{
    std::string line;
    while (!terminateKeyboard)
    {
        // Get a full line of input
        std::getline(std::cin, line);

        // Exit if the user types "exit_program"
        if (line == "exit_program")
        {
            break;
        }

        // Split the input into command and arguments
        std::vector<std::string> input = split_str(line, ' ');
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
                                               std::to_string(subscriptionId) + "\n"
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
                        Channel chan(channel_name);
                        channels.insert(std::make_pair(channel_name,chan));
                        subscriptions.insert(std::make_pair(channel_name,subscriptionId));
                        IncreamentRecieptId();
                        IncreamentSubId();
                    }
                }
            }
            else if (command == Command::EXIT)
            {
                if (input.size() < 2)
                {
                    std::cout << "Error: exit requires a channel name." << std::endl;
                    break;
                }
                else
                {
                    std::string channel_name = input[1];
                    std::string departmentName;
                    int subId;
                    auto it1 =channels.find(channel_name); // Find the key
                    auto it2 =subscriptions.find(channel_name);
                    if (it1 != channels.end()&&it2!=subscriptions.end())
                    {
                        Channel channel=it1->second;
                        subId=it2->second;
                        departmentName = channel.get_name(); // Access the value
                        std::cout << "Department ID: " << departmentName << std::endl;
                        std::string message_exit = "UNSUBSCRIBE\n"
                                          "id:" +
                                          std::to_string(subId) + "\n"
                                                                         "reciept" +
                                          std::to_string(recieptId) + "\0";
                    // Send the subscription message
                    if (!connection.sendLine(message_exit))
                    {
                        std::cerr << "Failed to send unsubscription message for channel:" << channel_name << std::endl;
                        break;
                    }
                    else
                    {    
                        channels.erase(channel_name);
                        IncreamentRecieptId();
                        //IncreamentSubId();
                        break;
                    }
                    }
                    else
                    {
                        std::cout << "Department not found!" << std::endl;
                        break;
                    }
                    
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
                    auto it =channels.find(channelName); // Find the key
                    if (it != channels.end())
                    {
                    Channel channel=it->second;
                    for (Event e : events)
                    {
                        std::string message_report = "SEND\n\n"
                                                     "user" +
                                                     connection.getLoginedUser() + "\n"
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
                        channel.addEvent(e.get_name(),e);
                        if(!connection.sendLine(message_report))
                        {
                        std::cerr << "Failed to send unsubscription message for channel:" << channelName << std::endl;
                        break;
                    }
                    else{}
                    }
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
                    terminateKeyboard = true;
                }
            }
            else if (command == Command::LOGIN) {
                cout << "The client is already logged in, log out before trying again" << endl << endl;
            }
            else
            {
                cout << "Command is invalid. Enter a new command" << endl;
            }
        }
    }
}


void InputStream::IncreamentRecieptId()
{
    recieptId++;
}
void InputStream::IncreamentSubId(){
    subscriptionId++;
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
