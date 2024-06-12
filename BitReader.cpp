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
    return currentSectorIndex + 1 < sector.size();
}

uint8_t BitReader::nextBit() {
    if (currentByteIndex == 7) {
        if (!hasNextByte()){
            std::cerr << BitReader::getCurrentSectorIndex() << std::endl;
            std::cerr << BitReader::getSectorSize() << std::endl;
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

uint16_t BitReader::nextNBits(int N){
    //TODO MAKE THIS MORE SECURE (-1 and N <= 0)
    uint16_t res = 0;
    for (int i = 0; i < N; i++){
        res <<= 1;
        auto nextBit = BitReader::nextBit();
        if (nextBit == 255) {
            std::cerr << "BIT READER FINISHED" << std::endl;
            break;
        } else {
            res += nextBit;
        }

    }
    return res;
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
            if (byte == 0xFF){
                uint8_t nextByte = sectorToTrim[i + 1];
                if (nextByte >= 0xD0 && nextByte <= 0xD7) {
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
                } else if (nextByte == 0xFE) {
                    //TODO Raise exception
                    std::cerr << "COM not supported" << std::endl;
                    break;
                } else {
                    std::cerr << "Unknown marker" << std::endl;
                    std::cerr << nextByte << std::endl;
                }
            } else {
                res.push_back(sectorToTrim[i]);
            }
        }
    }
    return res;
}

int BitReader::getSectorSize() {
    return sector.size();
}

int BitReader::getCurrentByteIndex() const {
    return currentByteIndex;
}

int BitReader::getCurrentSectorIndex() const {
    return currentSectorIndex;
}

bool BitReader::hasNextBit() {
    if (BitReader::hasNextByte()) {
        return true;
    } else {
        if (currentByteIndex < 7) {
            return true;
        } else {
            return false;
        }
    }

}

int BitReader::getCurrentByte() const {
    return (int) currentByte;
}
