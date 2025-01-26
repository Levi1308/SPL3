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

StompProtocol::StompProtocol(ConnectionHandler *conn) : terminateKeyboard(false), terminateServerResponses(false),
                                                        connection(conn), subscriptionId(0), subscriptions(), loggedIn(true), cv(), cvMutex(), waitframe()
{
}

void StompProtocol::runServerInput()
{
    while (true)
    {
        std::string answer, input;
        if(connection->isLogined())
        {
        connection->getLine(answer); // Receiving a response from the server
        /*
        if (!hasAnswered)
        { // If the server connection was closed and no response receieved - close the client
            std::cout << "Disconnected. Exiting...\n"
                      << std::endl;
            terminateKeyboard = true;
            break;
        }*/
        if (terminateServerResponses)
        {
            waitframe.push(answer);
        }
        else
        {
            while (!waitframe.empty())
            {
                std::string input = waitframe.front(); // Get the front element
                waitframe.pop();                       // Remove the front element
                processServerInput(input);             // Process the input
            }
            processServerInput(answer);
        }
        }
    }
}
void StompProtocol::processServerInput(string answer)
{
    StompFrame frame(answer);
    // cout << answer;
    if (frame.getCommand() == "CONNECTED")
    {
        cout << "Login succesful" << endl;
        cout << answer << endl;
    }
    if (frame.getCommand() == "RECEIPT")
    {
        map<string, string> headers = frame.getHeaders();
        string receiptID = headers.find("receipt-id")->second;
        string receiptCOMMAND = connection->findReceiptCommand(receiptID);
        if (receiptCOMMAND == "DISCONNECT")
        { // If the receipt received is for a DISCONNECT frame - close the connection
            cout << "Logged out" << endl;
            cout << "RECEIPT" << endl;
            cout << "recipet-id:" + receiptID << endl;
            terminateServerResponses = true;
            loggedIn = false;
            runServerInput();
        }
        else
        {
            cout << "RECEIPT" << endl;
            cout << "recipet-id:" + receiptID << endl;
        }
        cv.notify_all();
    }
    else if (frame.getCommand() == "ERROR")
    {
        // terminateKeyboard = true;
        terminateServerResponses = true;
        cout << frame.createFrame() << endl;
        connection->close();
        loggedIn = false;
        // delete connection;
    }
    else if (frame.getCommand() == "MESSAGE")
    { // If a message was received - a user sent a report and it needed to be saved
        string report = frame.getBody();
        vector<string> input = split(answer, '\n');
        string channel_name = "";
        for (string s : input)
        {
            if (s.find("destination:") == 0)
            {
                channel_name = s.substr(s.find(':') + 1);
            }
        }
        if (channel_name != "")
        {
            // cout<<"Channel message name:"+channel_name+"\n"<<endl;
            Event event = parseEventReport(report, channel_name);
            string user = event.get_name();
            connection->addReport(user, channel_name, event);
            cout << frame.getCommand() + "\n"
                 << endl;
            cout << frame.toStringHeaders() + "\n";
        }
        else
        {
            cout << "Frame missing destination" << endl;
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

Event StompProtocol::parseEventReport(string report, string channelName)
{
    std::vector<std::string> parts = split(report, '\n');
    std::string user, city, eventName, time, description, active, forceArrive;
    std::map<std::string, std::string> generalInformation;

    std::string previousLine;

    for (const auto &line : parts)
    {
        std::string trimmedLine = line;
        trimmedLine.erase(0, trimmedLine.find_first_not_of(" \t")); // Trim leading whitespace
        trimmedLine.erase(trimmedLine.find_last_not_of(" \t") + 1); // Trim trailing whitespace

        if (trimmedLine.find("user:") == 0)
        {
            user = trimmedLine.substr(trimmedLine.find(':') + 1);
        }
        else if (trimmedLine.find("city:") == 0)
        {
            city = trimmedLine.substr(trimmedLine.find(':') + 1);
        }
        else if (trimmedLine.find("event name:") == 0)
        {
            eventName = trimmedLine.substr(trimmedLine.find(':') + 1);
        }
        else if (trimmedLine.find("date time:") == 0)
        {
            time = trimmedLine.substr(trimmedLine.find(':') + 1);
        }
        else if (trimmedLine.find("active:") == 0)
        {
            active = trimmedLine.substr(trimmedLine.find(':') + 1);
        }
        else if (trimmedLine.find("forces_arrival_at_scene:") == 0)
        {
            forceArrive = trimmedLine.substr(trimmedLine.find(':') + 1);
        }
        else if (previousLine == "description:")
        {
            description = trimmedLine;
        }
        previousLine = trimmedLine;
    }

    generalInformation["active"] = active;
    generalInformation["forces_arrival_at_scene"] = forceArrive;

    // Validate the required fields
    if (user.empty() || city.empty() || eventName.empty() || time.empty() || description.empty() || generalInformation.empty())
    {
        throw std::runtime_error("Missing required fields in the report.");
    }

    // Parse time to integer
    int dateTime;
    try
    {
        dateTime = std::stoi(time);
    }
    catch (const std::invalid_argument &)
    {
        throw std::runtime_error("Invalid date time format in the report.");
    }
    Event event(channelName, city, eventName, dateTime, description, generalInformation);
    event.setEventOwnerUser(user);
    return event;
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
    while (true)
    {
        std::string line;
        if (!terminateKeyboard)
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
                            std::cout << "join command needs 1 arguments {channel_name}" << std::endl;
                        }
                        else
                        {
                            string join_command = "SUBSCRIBE";
                            string channel_name = input[1];
                            auto it = subscriptions.find(channel_name);
                            if (it == subscriptions.end())
                            {
                                int receipt = connection->produceReceipt(join_command);
                                map<string, string> headers;
                                headers.insert({"destination", channel_name});
                                headers.insert({"id", std::to_string(subscriptionId)});
                                headers.insert({"receipt", std::to_string(receipt)});

                                StompFrame frame(join_command, headers, "");
                                string output = frame.createFrame();
                                cout << output << endl;
                                // Send the subscription message
                                if (!connection->sendFrame(output))
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
                            else
                                cout << "User already joined this channel" << endl;
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
                            std::cout << "exit command needs 1 arguments {channel_name}" << std::endl;
                        }
                        else
                        {
                            string exit_command = "UNSUBSCRIBE";
                            string channel_name = input[1];
                            auto it = subscriptions.find(channel_name);
                            if (it != subscriptions.end())
                            {
                                int receipt = connection->produceReceipt(exit_command);
                                int subId = it->second;
                                map<string, string> headers;
                                headers.insert({"id", std::to_string(subId)});
                                headers.insert({"receipt", std::to_string(receipt)});
                                StompFrame frame(exit_command, headers, "");
                                string output = frame.createFrame();
                                cout << output << endl;
                                // Send the subscription message
                                if (!connection->sendFrame(output))
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
                        if (input.size() != 2)
                        {
                            std::cout << "join command needs 1 arguments {file_path}" << std::endl;
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
                            auto it = subscriptions.find(channelName);
                            if (it != subscriptions.end())
                            {
                                for (Event e : events)
                                {
                                    headers.insert({"destination", channelName});
                                    cout << "Started for loop" << endl;
                                    string body = "user:" + connection->getLoginedUser() + "\n";
                                    body += "city:" + e.get_city() + "\n";
                                    body += "event name:" + e.get_name() + "\n";
                                    body += "date time:" + std::to_string(e.get_date_time()) + "\n";
                                    body += "general information:\n";

                                    const auto &general_info = e.get_general_information();

                                    auto it = general_info.find("active");
                                    if (it != general_info.end())
                                    {
                                        body += "\tactive:" + it->second + "\n";

                                        it = general_info.find("forces_arrival_at_scene");
                                        if (it != general_info.end())
                                        {
                                            body += "\tforces_arrival_at_scene:" + it->second + "\n";
                                            body += "description:\n" + e.get_description() + "\n";
                                            StompFrame frame(report_command, headers, body);
                                            string output = frame.createFrame();
                                            cout << output << endl;
                                            if (!connection->sendFrame(output))
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
                            else
                            {
                                cout << "Channel does not found" << endl;
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
                            std::cout << "summary command requires {channel name}, {user}, {and file path}" << std::endl;
                        }
                        else
                        {
                            std::string channel_name = input[1];
                            cout << "channel:" + channel_name;
                            std::string user = input[2];
                            cout << "user:" + user;
                            std::string file_path = input[3];
                            cout << "file path:" + file_path;
                            auto it = subscriptions.find(channel_name);
                            if (it != subscriptions.end())
                            {
                                std::vector<Event> events = connection->getEventbyUser(channel_name, user);
                                for (Event e : events)
                                    cout << e.get_description() + "\n"
                                         << endl;
                                // Now you can process the events or write them to a file
                                if (!events.empty())
                                    writeToFile(file_path, channel_name, events); // Assuming `writeToFile` is implemented as above
                                else
                                    cout << "No Events to report" << endl;
                            }
                            else
                            {
                                std::cerr << "\nChannel not found: " << channel_name << std::endl;
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
                        int receipt = connection->produceReceipt(logout_command);
                        map<string, string> headers;
                        headers.insert({"receipt", to_string(receipt)});
                        StompFrame frame(logout_command, headers, "");
                        string output = frame.createFrame();
                        cout << output << endl;
                        if (connection->sendFrame(output))
                            subscriptions.clear();
                        else
                        {
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
                                string host = input[1].substr(0, input[1].find(':'));
                                short port = stoi(input[1].substr(input[1].find(':') + 1));
                                connection = new ConnectionHandler(host, port);
                                if (connection->connect())
                                {
                                    cout << "Win1" << endl;
                                    connection->connectUser(input[2]);
                                    if (connection->sendFrame(output))
                                    {
                                        cout << "Win2" << endl;
                                        loggedIn = true;
                                        cv.notify_all();
                                    }
                                    else
                                    {
                                        loggedIn = false;
                                        terminateServerResponses = true;
                                        cout << "Dont Win2" << endl;
                                        std::getline(std::cin, line);
                                        std::vector<std::string> input = split_str(line, ' ');
                                    }
                                }
                                else
                                {
                                    std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
                                    terminateServerResponses = true;
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
}

void StompProtocol::IncreamentSubId()
{
    subscriptionId++;
}
// Assume Event class has the appropriate methods defined
void StompProtocol::writeToFile(const std::string &file_path, const std::string &channel_name, std::vector<Event> events)
{
    // Helper function to convert epoch time to date-time string
    auto epoch_to_date = [](long long epoch_time) -> std::string
    {
        std::time_t time = static_cast<std::time_t>(epoch_time);
        std::tm *tm_ptr = std::gmtime(&time);
        std::ostringstream oss;
        oss << std::put_time(tm_ptr, "%d/%m/%y %H:%M");
        return oss.str();
    };

    // Open file for writing
    std::ofstream file(file_path, std::ios::trunc); // Overwrite the file
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

    for (const Event &event : events)
    {
        const auto &info = event.get_general_information();
        for (const auto &pair : info)
        {
            std::cout << pair.first << ": " << pair.second << std::endl;
        }
        auto it = info.find("active");
        if (it != info.end())
            if ((it->second).compare("true") == 0)
                active_count++;
        it = info.find("forces_arrival_at_scene");
        if (it != info.end())
            if ((it->second).compare("true") == 0)
                forces_arrival_count++;
    }

    // Write statistics
    file << "Stats:\n";
    file << "Total: " << total_reports << "\n";
    file << "Active: " << active_count << "\n";
    file << "Forces Arrival at Scene: " << forces_arrival_count << "\n";

    // Sort events by date_time and then by event_name
    std::sort(events.begin(), events.end(), [](const Event &a, const Event &b)
              {
        if (a.get_date_time() != b.get_date_time()) {
            return a.get_date_time() < b.get_date_time();
        }
        return a.get_name() < b.get_name(); });

    // Write event reports
    file << "Event Reports:\n";
    int report_number = 1;
    for (const Event &event : events)
    {
        file << "Report_" << report_number++ << ":\n";
        file << "\tCity: " << event.get_city() << "\n";
        file << "\tDate Time: " << epoch_to_date(event.get_date_time()) << "\n";
        file << "\tEvent Name: " << event.get_name() << "\n";

        // Truncate description for summary
        std::string summary = event.get_description();
        if (summary.length() > 27)
        {
            summary = summary.substr(0, 27) + "...";
        }
        file << "\tSummary: " << summary << "\n";
    }

    file.close();
    std::cout << "File written successfully to: " << file_path << std::endl;
}