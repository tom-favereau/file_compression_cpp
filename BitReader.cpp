//
// Created by alici on 05/06/2024.
//

#include "BitReader.h"

BitReader::BitReader(const std::vector<char>& sector) : sector(sector), currentByteIndex(-1), currentByte() , currentSectorIndex(-1){}

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
    char bit = (currentByte >> (7 - currentByteIndex)) & 1;
    return bit;
}