
#include <string>
#include <iostream>
#include <vector>
#include <cstddef> // For std::byte

class EncoderDecoder {
private:
    std::vector<std::byte> bytes; // Vector to store bytes
    int len = 0;             

public:
    // Constructor
    EncoderDecoder();

    // Method to decode the next byte
    std::string decodeNextByte(std::byte nextByte);

    // Method to encode data and return as a vector of bytes
    std::vector<std::byte> encode(std::string message);
};

