#pragma once

#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <map>
#include <list>

using boost::asio::ip::tcp;
using namespace std;

class Event;
class Channel;

class ConnectionHandler {
private:
	const std::string host_;
	const short port_;
	boost::asio::io_service io_service_;   // Provides core I/O functionality
	tcp::socket socket_;
	map<string,Channel> channels;
	string currentUser;
	bool logined; //Does the connection logined to a specific user
	int receiptNumber; //Generator of receipt numbers
	map<string, string> receipts; 
	

public:
    void connectUser(string user);
	void resetConnection(); //Disconnect the client from the user (update relevant data)
	string getLoginedUser(); //Get the logined user's name
	bool isLogined();
	int produceReceipt(string command); //generate the next receipt number
	string findReceiptCommand(string receiptID);
	void addReport(string user, string channel_name, Event report);
	bool getFrame(std::string &frame);
	bool sendFrame(std::string & frame);
	vector<Event> getEventbyUser(std::string channelname,std::string user);

	ConnectionHandler(std::string host, short port);

	virtual ~ConnectionHandler();

	// Connect to the remote machine
	bool connect();

	// Read a fixed number of bytes from the server - blocking.
	// Returns false in case the connection is closed before bytesToRead bytes can be read.
	bool getBytes(char bytes[], unsigned int bytesToRead);

	// Send a fixed number of bytes from the client - blocking.
	// Returns false in case the connection is closed before all the data is sent.
	bool sendBytes(const char bytes[], int bytesToWrite);

	// Read an ascii line from the server
	// Returns false in case connection closed before a newline can be read.
	bool getLine(std::string &line);

	// Send an ascii line from the server
	// Returns false in case connection closed before all the data is sent.
	bool sendLine(std::string &line);

	// Get Ascii data from the server until the delimiter character
	// Returns false in case connection closed before null can be read.
	bool getFrameAscii(std::string &frame, char delimiter);

	// Send a message to the remote host.
	// Returns false in case connection is closed before all the data is sent.
	bool sendFrameAscii(const std::string &frame, char delimiter);

	// Close down the connection properly.
	void close();

	ConnectionHandler& operator=(const ConnectionHandler& other);

}; //class ConnectionHandler
