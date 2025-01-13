#include <unordered_map>
#include <string>
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
    int departmentCounter;
    int recieptId;
    std::unordered_map<std::string,int> departments;
public:
    InputStream();
    void operator()(ConnectionHandler connection); 
    void IncreamentDepartments();
    void IncreamentRecieptId();
    void FileReaderJson(const std::string& configFilePath) ;
};
