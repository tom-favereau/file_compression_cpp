//
// Created by alici on 05/06/2024.
//

#ifndef FILE_COMPRESSION_CPP_BITREADER_H
#define FILE_COMPRESSION_CPP_BITREADER_H


#include <vector>
#include <cstdint>

class BitReader {
public:
    BitReader(const std::vector<char>& sector);
    uint8_t nextBit();
private:
    int currentByteIndex;
    char currentByte;
    int currentSectorIndex;
    std::vector<char> sector;

    bool hasNextByte();
};


#endif //FILE_COMPRESSION_CPP_BITREADER_H
