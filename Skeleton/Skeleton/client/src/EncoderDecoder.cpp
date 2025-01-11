

#include "EncoderDecoder.h"

#include <string>
#include <iostream>
#include <vector>
#include <cstddef> 

class EncoderDecoder {
private:
    std::vector<std::byte> bytes; // Dynamic byte array
    int len = 0;                  // Current length

public:
    EncoderDecoder() {}

    // Decodes the next byte, returning a string when '\n' is encountered
    std::string decodeNextByte(std::byte nextByte) {
        if (nextByte == std::byte{'\n'}) {
            std::string ret(bytes.begin(), bytes.end());
            bytes.clear(); 
            len = 0;       
            return ret;
        }

        bytes.push_back(nextByte);
        len++;
        return ""; 
    }

    std::vector<std::byte> encode(std::string message){
    std::vector<std::byte> bytes;
    for (char c : message) {
        bytes.push_back(static_cast<std::byte>(c));
    }
    return bytes;
    }
};

