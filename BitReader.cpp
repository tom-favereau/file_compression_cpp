//
// Created by alici on 05/06/2024.
//

#include <iostream>
#include "BitReader.h"

BitReader::BitReader(const std::vector<char>& sector) : currentByteIndex(-1), currentSectorIndex(0){
    this->sector = trim(sector);
    currentByte = this->sector[0];
}

bool BitReader::hasNextByte(){
    return currentSectorIndex < sector.size();
}

uint8_t BitReader::nextBit() {
    if (currentByteIndex == 7) {
        if (!hasNextByte()){
            return 255;
        } else {
            currentSectorIndex++;
            currentByte = sector[currentSectorIndex];
        }
    }
    currentByteIndex = (currentByteIndex + 1) % 8;
    uint8_t bit = (currentByte >> (7 - currentByteIndex)) & 1;
    return bit;
}

//TODO TEST
std::vector<uint8_t> BitReader::trim(const std::vector<char>& sectorToTrim) {
    std::vector<uint8_t> res;
    bool skipNext = false;
    for (int i = 0; i < sectorToTrim.size(); i++) {
        if (skipNext) {
            skipNext = false;
        } else {
            uint8_t byte = sectorToTrim[i];
            if (byte == 0xff){
                uint8_t nextByte = sectorToTrim[i + 1];
                if (nextByte >= 0xd0 && nextByte <= 0xd7) {
                    //TODO Raise exception
                    std::cerr << "DRI not supported" << std::endl;
                    break;
                } else if (nextByte == 0x00) {
                    skipNext = true;
                    res.push_back(sectorToTrim[i]);
                } else if (nextByte == 0xFF) {
                    //TODO Raise exception
                    std::cerr << "Multiple 0xff not supported" << std::endl;
                    break;
                }
            } else {
                res.push_back(sectorToTrim[i]);
            }
        }
    }
    return res;
}
