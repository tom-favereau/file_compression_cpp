//
// Created by alici on 01/06/2024.
//

#ifndef FILE_COMPRESSION_CPP_BYTEREADING_H
#define FILE_COMPRESSION_CPP_BYTEREADING_H


#include <vector>

/**
 * Facilitation of reading bytes in sector (vector of char) or reading bits within a byte.
 */
class ByteReading {

public:
    static int readBytes(const std::vector<char>& imageBytes, int index, int size);
    static int readByte(char byte, int index, int size);
};


#endif //FILE_COMPRESSION_CPP_BYTEREADING_H
