#include <unordered_map>
#include <string>
#include <map>
#include "ConnectionHandler.h"
#include "Channel.h"
#include "keyboardInput.h"
// Enum for commands
enum class Command {
    LOGIN,
    JOIN,
    EXIT,
    REPORT,
    SUMMARY,
    UNKNOWN,
    LOGOUT
};
using namespace std;
class InputStream {
private:
    std::map<std::string,Channel> channels;
    int subscriptionId;
    std::map<std::string,int> subscriptions;
    bool terminateKeyboard;


public:
    InputStream();
    void run(ConnectionHandler& connection);
    void IncreamentSubId();
    void writeToFile(const std::string &file_path, const std::string &channel_name, const std::vector<Event> &events);
};
