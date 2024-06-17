//
// Created by alici on 01/06/2024.
//

#include "ByteReading.h"

/**
     * Read bytes in a char vector and converts them to an int.
     * @param imageBytes char vector which is read.
     * @param index index at which reading starts.
     * @param size number of bytes read to convert to an int.
     * @return the int represented by the bytes starting at index index of size size.
     */
int ByteReading::readBytes(const std::vector<char>& imageBytes, int index, int size){
    int res = 0;
    for (int i = 0; i < size; i++) {
        res |= (imageBytes[i + index] & 0xFF) << ((size - 1 - i) * 8);
    }
    return res;
}

/**
 * Read bits in a byte and converts the result to an int.
 * @param byte char to read from.
 * @param index index in the byte at which reading is started.
 * @param size number of bits to be read.
 * @return the int represented by the bits starting at index index of size size.
 */
int ByteReading::readByte(char byte, int index, int size){
    char mask = ((1 << size) - 1) << (8 - index - size);
    return (byte & mask) >> (8 - index - size);
}