#include <string>
#include <iostream>
#include <vector>

class EncoderDecoder {
private:
    std::vector<unsigned char> bytes; // Vector to store bytes
    int len = 0;                      // Length of the data

public:
    // Constructor
    EncoderDecoder();

    // Method to decode the next byte
    std::string decodeNextByte(unsigned char nextByte);

    // Method to encode data and return as a vector of bytes
    std::vector<unsigned char> encode(const std::string& message);
};
