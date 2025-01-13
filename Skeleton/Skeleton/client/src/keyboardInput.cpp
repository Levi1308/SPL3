
#include "keyboardInput.h"

#include <string>
#include <vector>
#include <sstream>

static std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str); // Create a stringstream from the input string
    std::string token;

    // Use getline to split the string by the delimiter
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token); // Add each token to the vector
    }

    return tokens;
}