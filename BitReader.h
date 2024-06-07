//
// Created by alici on 05/06/2024.
//

#ifndef FILE_COMPRESSION_CPP_BITREADER_H
#define FILE_COMPRESSION_CPP_BITREADER_H


#include <vector>
#include <cstdint>

class BitReader {
public:
    explicit BitReader(const std::vector<char>& sector);
    uint8_t nextBit();

    uint16_t nextNBits(int N);
    bool hasNextByte();
    int getSectorSize();
private:
    int currentByteIndex;
    uint8_t currentByte;
    int currentSectorIndex;
    std::vector<uint8_t> sector;
    static std::vector<uint8_t> trim(const std::vector<char>& sector);

};


#endif //FILE_COMPRESSION_CPP_BITREADER_H
