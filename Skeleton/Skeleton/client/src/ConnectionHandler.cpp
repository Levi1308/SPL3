#include "../include/ConnectionHandler.h"
#include "../include/event.h"
#include "../include/Channel.h"

using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

ConnectionHandler::ConnectionHandler(string host, short port) : host_(host), port_(port), io_service_(),
                                                                socket_(io_service_),channels(),  currentUser(), logined(false),
																 receiptNumber(0), receipts(), subscriptions() {}


ConnectionHandler::~ConnectionHandler() {
	close();
}

bool ConnectionHandler::connect() {
	std::cout << "Starting connect to "
	          << host_ << ":" << port_ << std::endl;
	try {
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
		boost::system::error_code error;
		socket_.connect(endpoint, error);
		if (error)
			throw boost::system::system_error(error);
	}
	catch (std::exception &e) {
		std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
		return false;
	}
	return true;
}

bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
	size_t tmp = 0;
	boost::system::error_code error;
	try {
		while (!error && bytesToRead > tmp) {
			tmp += socket_.read_some(boost::asio::buffer(bytes + tmp, bytesToRead - tmp), error);
		}
		if (error)
			throw boost::system::system_error(error);
	} catch (std::exception &e) {
		std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
		return false;
	}
	return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
	int tmp = 0;
	boost::system::error_code error;
	try {
		while (!error && bytesToWrite > tmp) {
			tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
		}
		if (error)
			throw boost::system::system_error(error);
	} catch (std::exception &e) {
		std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
		return false;
	}
	return true;
}

bool ConnectionHandler::getLine(std::string &line) {
	return getFrameAscii(line, '\n');
}

bool ConnectionHandler::sendLine(std::string &line) {
	return sendFrameAscii(line, '\n');
}


bool ConnectionHandler::getFrameAscii(std::string &frame, char delimiter) {
	char ch;
	// Stop when we encounter the null character.
	// Notice that the null character is not appended to the frame string.
	try {
		do {
			if (!getBytes(&ch, 1)) {
				return false;
			}
			if (ch != '\0')
				frame.append(1, ch);
		} while (delimiter != ch);
	} catch (std::exception &e) {
		std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
		return false;
	}
	return true;
}

bool ConnectionHandler::sendFrameAscii(const std::string &frame, char delimiter) {
	bool result = sendBytes(frame.c_str(), frame.length());
	if (!result) return false;
	return sendBytes(&delimiter, 1);
}

// Close down the connection properly.
void ConnectionHandler::close() {
	try {
		socket_.close();
	} catch (...) {
		std::cout << "closing failed: connection already closed" << std::endl;
	}
	/*
	ConnectionHandler& ConnectionHandler::operator=(const ConnectionHandler& other){
		if(this!=&other)
		{
			port_=other.port_;

		}
		return this;
	}
	*/
}

void ConnectionHandler::connectUser(string user){
	currentUser = user;
	logined = true;
}

void ConnectionHandler::resetConnection()
{
	currentUser = "";
	receiptNumber = 0;
	subscriptions.clear();
	receipts.clear();
	channels.clear();
	logined = false;
}

string ConnectionHandler::getLoginedUser()
{
	return currentUser;
}

bool ConnectionHandler::isLogined()
{
	return this->logined;
}

string ConnectionHandler::findReceiptCommand(string receiptID) {
	return receipts.find(receiptID)->second;
}

int ConnectionHandler::getSubID(string channel) {
	if (subscriptions.find(channel) != subscriptions.end()) {
		return subscriptions.find(channel)->second;
	}
	else {
		return -1;
	}
}

int ConnectionHandler::insertSub(string channel) {
	int subID;
	if (subscriptions.empty()) {
		subID = 0;
	}
	else {
		subID = subscriptions.size();
	}
	subscriptions.insert({channel, subID});
	return subID;
}

int ConnectionHandler::produceReceipt(string command) {
	int receipt = receiptNumber;
	receiptNumber++;
	receipts.insert({to_string(receipt), command});
	return receipt;
}

void ConnectionHandler::addReport(string user, string channel_name, Event report){
	if (channels.find(channel_name) == channels.end()) {
		Channel channel(channel_name);
		channel.addEvent(user, report);
		channels.insert({ channel_name, channel });
	}
	else {
		channels.find(channel_name)->second.addEvent(user, report);
	}
}

bool ConnectionHandler::getFrame(std::string &frame) {
	return getFrameAscii(frame, '\0');
}

bool ConnectionHandler::sendFrame(std::string & frame) {
	return sendFrameAscii(frame, '\0');
}




