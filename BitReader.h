//
// Created by alici on 05/06/2024.
//

#ifndef FILE_COMPRESSION_CPP_BITREADER_H
#define FILE_COMPRESSION_CPP_BITREADER_H


#include <vector>
#include <cstdint>
/**
 * Class that allows reading a byte stream bit by bit.
 * 4 attributes:
 *  - currentByteIndex (current index in the byte currently read)
 *  - currentByte (byte which is currently read)
 *  - sector: byte stream to be read given as a vector of uint8_t
 *  - currentSectorIndex: index of currently read byte in the sector
 */
class BitReader {
public:
    explicit BitReader(const std::vector<char>& sector);
    uint8_t nextBit();

    uint16_t nextNBits(int N);
    bool hasNextByte();
    bool hasNextBit();
    int getSectorSize();
    int getCurrentByteIndex() const;
    int getCurrentSectorIndex() const;
    int getCurrentByte() const;
private:
    int currentByteIndex;
    uint8_t currentByte;
    int currentSectorIndex;
    std::vector<uint8_t> sector;
    static std::vector<uint8_t> trim(const std::vector<char>& sector);

};


#endif //FILE_COMPRESSION_CPP_BITREADER_H
