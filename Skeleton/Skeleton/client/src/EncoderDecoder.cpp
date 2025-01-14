#include <vector>
#include <string>
#include <iostream>

class EncoderDecoder {
private:
    std::vector<unsigned char> bytes; // Dynamic byte array (using unsigned char)
    int len = 0;                      // Current length

public:
    EncoderDecoder():bytes(){};

    // Decodes the next byte, returning a string when '\n' is encountered
    std::string decodeNextByte(unsigned char nextByte) {
        if (nextByte == '\n') {
            std::string ret(bytes.begin(), bytes.end());
            bytes.clear();
            len = 0;
            return ret;
        }

        bytes.push_back(nextByte);
        len++;
        return ""; 
    }

    std::vector<unsigned char> encode(std::string message) {
        std::vector<unsigned char> bytes;
        for (char c : message) {
            bytes.push_back(static_cast<unsigned char>(c)); // Convert to unsigned char
        }
        return bytes;
    }
};
