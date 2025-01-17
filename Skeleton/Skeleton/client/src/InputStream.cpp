

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
#include "StompFrame.h"

InputStream::InputStream()
    : channels(), subscriptionId(0), subscriptions(), terminateKeyboard(false) {};

// Map to associate string commands with enum values
const std::unordered_map<std::string, Command> commandMap = {
    {"login", Command::LOGIN},
    {"logout", Command::LOGOUT},
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

void InputStream::run(ConnectionHandler &connection)
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
                    string join_command = "SUBSCRIBE";
                    string channel_name = input[1];
                    int receipt = connection.produceReceipt(join_command);
                    map<string, string> headers;
                    headers.insert({"destination", channel_name});
                    headers.insert({"id", std::to_string(subscriptionId)});
                    headers.insert({"reciept", std::to_string(receipt)});

                    StompFrame frame(join_command, headers, "");
                    string output = frame.createFrame();
                    cout<<output<<endl;
                    // Send the subscription message
                    if (!connection.sendFrame(output))
                    {
                        std::cerr << "Failed to send subscription message for channel:" << channel_name << std::endl;
                        continue;
                    }
                    else
                    {
                        // Channel chan(channel_name);
                        // channels.insert(std::make_pair(channel_name,chan));
                        subscriptions.insert(std::make_pair(channel_name, subscriptionId));
                        IncreamentSubId();
                    }
                }
            }
            else if (command == Command::EXIT)
            {
                if (input.size() < 2)
                {
                    std::cout << "Error: exit requires a channel name." << std::endl;
                    //break;
                }
                else
                {

                    string exit_command = "UNSUBSCRIBE";
                    int receipt = connection.produceReceipt(exit_command);
                    string channel_name = input[1];
                    auto it = subscriptions.find(channel_name);
                    if (it != subscriptions.end())
                    {
                        int subId = it->second;
                        map<string, string> headers;
                        headers.insert({"id", std::to_string(subId)});
                        headers.insert({"reciept", std::to_string(receipt)});
                        StompFrame frame(exit_command, headers, "");
                        string output = frame.createFrame();
                        cout<<output<<endl;
                        // Send the subscription message
                        if (!connection.sendFrame(output))
                        {
                            std::cerr << "Failed to send unsubscription message for channel:" << channel_name << std::endl;
                            //break;
                        }
                        else
                        {
                            // channels.erase(channel_name);
                        }
                    }
                    else
                    {
                        std::cout << "channel not found!" << std::endl;
                        //break;
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
                    string report_command = "REPORT";
                    map<string, string> headers;
                    string file_path = input[1];
                    // Parse the events file
                    names_and_events parsedData = parseEventsFile(file_path);
                    // Extract channel name and events
                    std::string channelName = parsedData.channel_name;
                    std::vector<Event> events = parsedData.events;
                    for (Event e : events)
                    {
                        headers.insert({"user", connection.getLoginedUser()});
                        headers.insert({"city", e.get_city()});
                        headers.insert({"event name", e.get_name()});
                        headers.insert({"date time", std::to_string(e.get_date_time())});
                        headers.insert({"general information", ""});
                        headers.insert({"general information", ""});
                        const auto &general_info = e.get_general_information();

                        auto it = general_info.find("active");
                        if (it != general_info.end())
                        {
                            headers.insert({"active", it->second});
                            it = general_info.find("forces_arrival_at_scene");
                            if (it != general_info.end())
                            {
                                headers.insert({"forces_arrival_at_scene", it->second});
                                headers.insert({"description", e.get_description()});
                                StompFrame frame(report_command, headers, "");
                                string output = frame.createFrame();
                                cout<<output<<endl;
                                if (!connection.sendFrame(output))
                                {
                                    std::cerr << "Failed to send unsubscription message for channel:" << channelName << std::endl;
                                    //break;
                                }
                                else
                                {
                                    std::cout << "Report send correctly" << std::endl;
                                }
                            }
                            else
                                std::cout << "Json doesn't include forces_arrival_at_scene" << std::endl;
                            // break;
                        }
                        else
                        {
                            std::cout << "Json doesn't include active" << std::endl;
                            // break;
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
                /* string logout_command = "DISCONNECT";
                int receipt = connection.produceReceipt(logout_command);
                map<string, string> headers;
                headers.insert({ "receipt", to_string(receipt)});
                StompFrame frame(logout_command, headers, "");
                string output = frame.createFrame();
                connection.sendFrame(output);
                terminateKeyboard = true;
                */
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
                string logout_command = "DISCONNECT";
                int receipt = connection.produceReceipt(logout_command);
                map<string, string> headers;
                headers.insert({"receipt", to_string(receipt)});
                StompFrame frame(logout_command, headers, "");
                string output = frame.createFrame();
                connection.sendFrame(output);
                terminateKeyboard = true;
            }
            else if (command == Command::LOGIN)
            {
                cout << "The client is already logged in, log out before trying again" << endl
                     << endl;
            }
            else
            {
                std::cout << "Command is invalid. Enter a new command" << std::endl;
            }
        }
    }
}

void InputStream::IncreamentSubId()
{
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
