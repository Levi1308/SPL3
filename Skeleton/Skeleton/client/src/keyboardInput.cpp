
#include "keyboardInput.h"
#include <sstream> // Required for std::stringstream

// Function to split a string by a delimiter
std::vector<std::string> split_str(const std::string str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str); // Create a stringstream from the input string
    std::string token;

    // Use getline to split the string by the delimiter
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token); // Add each token to the vector
    }

    return tokens;
}
