#include "StompProtocol.h"
#include "../include/event.h"
#include <iostream>
#include <sstream> // For stringstream operations
#include "keyboardInput.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "ConnectionHandler.h"
#include <iostream>
#include <fstream>
#include <string>
#include <json.hpp>
#include "event.h"
#include "keyboardInput.h"
#include "StompFrame.h"
#include "Channel.h"
#include <thread>

using namespace std;

StompProtocol::StompProtocol(ConnectionHandler &conn) : terminateKeyboard(false), terminateServerResponses(false),
                                                        connection(conn), subscriptionId(0), subscriptions(), loggedIn(true), cv(), cvMutex()
{
}

void StompProtocol::runServerInput()
{
    while (!terminateServerResponses)
    {

        std::unique_lock<std::mutex> lock(cvMutex);

        cv.wait(lock, [this]
                { return loggedIn; });

        lock.unlock();

        std::string answer;
        bool hasAnswered = connection.getLine(answer); // Receiving a response from the server
        if (!hasAnswered)
        { // If the server connection was closed and no response receieved - close the client
            std::cout << "Disconnected. Exiting...\n"
                      << std::endl;
            terminateKeyboard = true;
            break;
        }

        if (hasAnswered)
        { // If a response from the server received
            StompFrame frame(answer);
            cout << answer;
            if (frame.getCommand() == "CONNECTED")
            {
                cout << "Login succesful" << endl;
            }
            if (frame.getCommand() == "RECEIPT")
            {
                map<string, string> headers = frame.getHeaders();
                string receiptID = headers.find("receipt-id")->second;
                string receiptCOMMAND = connection.findReceiptCommand(receiptID);
                if (receiptCOMMAND == "DISCONNECT")
                { // If the receipt received is for a DISCONNECT frame - close the connection
                    cout << "Logged out" << endl;
                    cout << "RECEIPT" << endl;
                    cout << "recipet-id:" + receiptID << endl;
                    loggedIn = false;
                    std::lock_guard<std::mutex> lock(cvMutex);
                }
                cv.notify_all();
            }
            else if (frame.getCommand() == "ERROR")
            {
                terminateKeyboard = true;
                terminateServerResponses = true;
                connection.close();
            }
            else if (frame.getCommand() == "MESSAGE")
            { // If a message was received - a user sent a report and it needed to be saved
                string report = frame.getBody();
                vector<string> lines = split_str(report, '\n');
                string userLine = lines[0];
                int index = userLine.find(':') + 1;
                string user = userLine.substr(index);
                Event event = parseEventReport(report);
                string channel_name = event.get_channel_name();
                connection.addReport(user, channel_name, event);
            }
        }
    }
}

vector<string> StompProtocol::split(string line, char delimiter)
{
    vector<string> parts;
    while (line.size() > 0)
    {
        string part = "";
        std::size_t i = 0;
        for (i = 0; i < line.size(); i++)
        {
            if (delimiter == line[i])
            {
                if (part.size() > 0)
                    parts.push_back(part);
                break;
            }
            else
                part += line[i];
        }
        if (i == line.size())
        {
            parts.push_back(part);
            line = "";
        }
        else
            line = line.substr(i + 1);
    }
    return parts;
}

Event StompProtocol::parseEventReport(string report)
{
    vector<string> parts = StompProtocol::split(report, '\n');
    string user, channel_name, city, event_name, time, description;
    map<string, string> general_information;
    bool details = true, general = false;

    string previousLine;

    for (const string &line : parts)
    {
        if (details)
        {
            if (line.find("channel name:") != string::npos)
            {
                size_t index = line.find(':');
                channel_name = line.substr(index + 1);
            }
            else if (line.find("city:") != string::npos)
            {
                size_t index = line.find(':');
                city = line.substr(index + 1);
            }
            else if (line.find("event name:") != string::npos)
            {
                size_t index = line.find(':');
                event_name = line.substr(index + 1);
            }
            else if (line.find("data time:") != string::npos)
            {
                size_t index = line.find(':');
                time = line.substr(index + 1);
            }
        }

        if (line == "general information:")
        {
            details = false;
        }

        if (general)
        {
            size_t index = line.find(':');
            if (index != string::npos)
            {
                string header = line.substr(0, index);
                string value = line.substr(index + 1);
                general_information[header] = value;
            }
        }
        else if (previousLine == "description:")
        {
            description = line;
        }

        previousLine = line;
    }

    // Validate the required fields
    if (channel_name.empty() || city.empty() || event_name.empty() || time.empty() || description.empty())
    {
        throw runtime_error("Missing required fields in the report.");
    }

    // Create and return the Event object
    int date_time = stoi(time);
    return Event(channel_name, city, event_name, date_time, description, general_information);
}
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

void StompProtocol::runkeyboardInput()
{

    std::string line;
    while (!terminateKeyboard)
    {
        // Get a full line of input
        std::getline(std::cin, line);

        // Exit if the user types "exit_program"
        if (line == "exit_program")
        {
            terminateKeyboard = true;
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
                if (loggedIn)
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
                        headers.insert({"receipt", std::to_string(receipt)});

                        StompFrame frame(join_command, headers, "");
                        string output = frame.createFrame();
                        cout << output << endl;
                        // Send the subscription message
                        if (!connection.sendFrame(output))
                        {
                            std::cerr << "Failed to send subscription message for channel:" << channel_name << std::endl;
                            continue;
                        }
                        else
                        {
                            subscriptions.insert({channel_name, subscriptionId});
                            IncreamentSubId();
                        }
                    }
                }
                else
                {
                    cout << "Login first" << endl;
                }
            }
            else if (command == Command::EXIT)
            {
                if (loggedIn)
                {
                    if (input.size() < 2)
                    {
                        std::cout << "Error: exit requires a channel name." << std::endl;
                        // break;
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
                            headers.insert({"receipt", std::to_string(receipt)});
                            StompFrame frame(exit_command, headers, "");
                            string output = frame.createFrame();
                            cout << output << endl;
                            // Send the subscription message
                            if (!connection.sendFrame(output))
                            {
                                std::cerr << "Failed to send unsubscription message for channel:" << channel_name << std::endl;
                                // break;
                            }
                            else
                            {
                                subscriptions.erase(channel_name);
                            
                            }
                        }
                        else
                        {
                            std::cout << "channel not found!" << std::endl;
                            // break;
                        }
                    }
                }
                else
                {
                    cout << "Login first" << endl;
                }
            }
            else if (command == Command::REPORT)
            {
                if (loggedIn)
                {
                    if (input.size() < 2)
                    {
                        std::cout << "Error: report requires a file path." << std::endl;
                    }
                    else
                    {
                        string report_command = "SEND";
                        map<string, string> headers;
                        string file_path = input[1];
                        // Parse the events file
                        names_and_events parsedData = parseEventsFile(file_path);
                        // Extract channel name and events
                        std::string channelName = parsedData.channel_name;
                        std::vector<Event> events = parsedData.events;
                        
                        for (Event e : events)
                        {
                            headers.insert({"destination",channelName });
                            cout<<"Started for loop"<<endl;
                            string body="user:"+connection.getLoginedUser()+"\n";
                            body+="city:"+e.get_city()+"\n";
                            body+="event name:"+ e.get_name()+"\n";
                            body+="date time:"+ std::to_string(e.get_date_time())+"\n";
                            body+="general information:\n";
                            
                            const auto &general_info = e.get_general_information();

                            auto it = general_info.find("active");
                            if (it != general_info.end())
                            {
                                body+="\tactive:"+it->second+"\n";

                                it = general_info.find("forces_arrival_at_scene");
                                if (it != general_info.end())
                                {
                                    body+="\tforces_arrival_at_scene:"+ it->second+"\n";
                                    body+="description:\n"+ e.get_description()+"\n";
                                    StompFrame frame(report_command, headers, body);
                                    string output = frame.createFrame();
                                    cout << output << endl;
                                    if (!connection.sendFrame(output))
                                    {
                                        std::cerr << "Failed to send unsubscription message for channel:" << channelName << std::endl;
                                        // break;
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
                            headers.clear();
                        }
                    }
                }
                else
                {
                    cout << "Login first" << endl;
                }
            }
            else if (command == Command::SUMMARY)
            {
                if (loggedIn)
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
                        auto it = subscriptions.find(channel_name);
                        if (it != subscriptions.end())
                        {
                          std::vector<Event> events = connection.getEventbyUser(channel_name,user);
                            // Now you can process the events or write them to a file
                        if(!events.empty())
                           writeToFile(file_path, channel_name, events); // Assuming `writeToFile` is implemented as above
                        else
                            cout << "No Events to report" << endl;
                        }
                        else
                        {
                            std::cerr << "Channel not found: " << channel_name << std::endl;
                        }
                    }
                }
                else
                {
                    cout << "Login first" << endl;
                }
            }
            else if (command == Command::LOGOUT)
            {
                if (loggedIn)
                {
                    string logout_command = "DISCONNECT";
                    int receipt = connection.produceReceipt(logout_command);
                    map<string, string> headers;
                    headers.insert({"receipt", to_string(receipt)});
                    StompFrame frame(logout_command, headers, "");
                    string output = frame.createFrame();
                    cout<<output<<endl;
                    if(connection.sendFrame(output))
                        subscriptions.clear();
                    else{
                        cout << "Couldnot log out" << endl;
                    }
                }
                else
                {
                    cout << "Login first" << endl;
                }
            }
            else if (command == Command::LOGIN)
            {
                if (loggedIn)
                {
                    cout << "The client is already logged in. Log out before trying again." << endl;
                }
                else
                {
                    
                    if (input.size() == 4)
                    {
                        while (!loggedIn)
                        {
                            string command = "CONNECT";
                            map<string, string> headers;
                            headers.insert({"accept-version", "1.2"});
                            headers.insert({"host", "stomp.cs.bgu.ac.il"});
                            headers.insert({"login", input[2]});
                            headers.insert({"passcode", input[3]});
                            StompFrame frame(command, headers, "");
                            string output = frame.createFrame();
                            cout << output << endl;
                            connection.connectUser(input[2]);
                            if (connection.sendFrame(output))
                                loggedIn = true;
                            else
                            {
                            std::getline(std::cin, line);
                            std::vector<std::string> input = split_str(line, ' ');
                            }
                        }
                    }
                }
            }
            else
            {
                std::cout << "Command is invalid. Enter a new command" << std::endl;
            }
        }
    }
}

void StompProtocol::IncreamentSubId()
{
    subscriptionId++;
}
void StompProtocol::writeToFile(const std::string &file_path, const std::string &channel_name, const std::vector<Event> &events)
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
