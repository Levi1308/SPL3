
#include "ConnectionHandler.h"
// Enum for commands
enum class Command {
    LOGIN,
    JOIN,
    EXIT,
    REPORT,
    SUMMARY,
    UNKNOWN
};

class InputStream {
private:
    int subscribeCounter;
public:
    InputStream(){};
    void operator()(ConnectionHandler connection); 
    void IncreamentSubscribers();
};
