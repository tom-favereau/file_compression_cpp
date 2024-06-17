//
// Created by alici on 05/06/2024.
//

#include <iostream>
#include "BitReader.h"

/**
 * Constructor, creates an instance of bit reader from a sector
 * @param sector : sector from which bytes are read bit by bit, given as a vector of char.
 */
BitReader::BitReader(const std::vector<char>& sector) : currentByteIndex(-1), currentSectorIndex(0){
    // In nextBit, currentByteIndex will always be incremented modulo 8, thus the minus one.
    this->sector = trim(sector);
    currentByte = this->sector[0];
}

/**
 * Tests if a bit reader has a next byte to be read.
 * @return true if there is a next byte, false otherwise.
 */
bool BitReader::hasNextByte(){
    return currentSectorIndex + 1 < sector.size();
}

/**
 * Returns the next bit to be read in the sector.
 * @return Returns the next bit, i.e. 0 or 1 as an uint8_t (so there 7 zeroes first).
 */
uint8_t BitReader::nextBit() {
    //If last bit read is at the end of a byte, the next byte is called
    if (currentByteIndex == 7) {
        if (!hasNextByte()){
            //If the end is reached (shouldn't happen in use cases, return 11111111).
            std::cerr << BitReader::getCurrentSectorIndex() << std::endl;
            std::cerr << BitReader::getSectorSize() << std::endl;
            return 255;
        } else {
            currentSectorIndex++;
            currentByte = sector[currentSectorIndex];
        }
    }
    //Increment byte index
    currentByteIndex = (currentByteIndex + 1) % 8;
    //Shift the bit to the right and put 0 in the first seven bits.
    uint8_t bit = (currentByte >> (7 - currentByteIndex)) & 1;
    return bit;
}

/**
 * Returns the next N bits of the bit reader.
 * @param N int: number of bits to be read at once.
 * @return the next N bits as an uint16_t because in use cases, 16 bits will be read at once at most.
 */
uint16_t BitReader::nextNBits(int N){
    //Principle: call N times nextBit
    //TODO MAKE THIS MORE SECURE (-1 and N <= 0)
    uint16_t res = 0;
    for (int i = 0; i < N; i++){
        res <<= 1;
        auto nextBit = BitReader::nextBit();
        //Should the end of the byte stream be reached, print a message
        if (nextBit == 255) {
            std::cerr << "BIT READER FINISHED" << std::endl;
            break;
        } else {
            res += nextBit;
        }

    }
    return res;
}

/**
 * Static function to trim the huffman encoded byte stream from byte stuffing.
 * @param sectorToTrim Huffman encoded byte stream given as a vector of char.
 * @return trimmed sector as a vector of uint8_t.
 */
std::vector<uint8_t> BitReader::trim(const std::vector<char>& sectorToTrim) {
    std::vector<uint8_t> res;
    bool skipNext = false;
    int count = 0;
    for (int i = 0; i < sectorToTrim.size(); i++) {
        if (skipNext) {
            //Skipping current byte
            skipNext = false;
            count++;
        } else {
            uint8_t byte = sectorToTrim[i];
            //If current byte is 0xFF, there is potential byte stuffing or a marker.
            if (byte == 0xFF){
                uint8_t nextByte = sectorToTrim[i + 1];
                //Reset Intervals are not supported
                if (nextByte >= 0xD0 && nextByte <= 0xD7) {
                    //TODO Raise exception
                    std::cerr << "DRI not supported" << std::endl;
                    break;
                //In case of byte stuffing, skip next byte
                } else if (nextByte == 0x00) {
                    skipNext = true;
                    res.push_back(sectorToTrim[i]);
                //Doesn't happen often but multiple 0xff in a row can occur and should be ignored,
                //but this in supported yet.
                } else if (nextByte == 0xFF) {
                    //TODO Raise exception
                    std::cerr << "Multiple 0xff not supported" << std::endl;
                    break;
                //For debugging purposes, check if a COM section was missed.
                } else if (nextByte == 0xFE) {
                    //TODO Raise exception
                    std::cerr << "COM not supported" << std::endl;
                    break;
                //Unknown marker
                } else {
                    std::cerr << "Unknown marker" << std::endl;
                    std::cerr << nextByte << std::endl;
                }
            } else {
                res.push_back(sectorToTrim[i]);
            }
        }
    }
    std::cout << "Trimmed " << count << std::endl;
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

/**
 * Checks if a bit reader has a next byte to be read.
 * @return true if a bit reader has a next byte to be read, false otherwise.
 */
bool BitReader::hasNextBit() {
    //If there is a next Byte to be read, a fortiori, there is a next bit to be read
    if (BitReader::hasNextByte()) {
        return true;
    } else {
        //Check is for the last byte, the last bit was reached.
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
