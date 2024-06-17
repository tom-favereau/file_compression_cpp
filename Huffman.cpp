//
// Created by Tom Favereau on 22/05/2024.
//

#include "Huffman.h"
#include <vector>
#include "ByteReading.h"

/**
 * Constructs a Huffman instance from a DHT sector.
 * @param sector vector of char of a DHT section, passed by reference.
 */
Huffman::Huffman(const std::vector<char>& sector){
    type = ByteReading::readByte(sector[4], 0, 4);
    indice_huffman = ByteReading::readByte(sector[4], 4, 4);
    for (int i = 0; i < 16; i++){
        nb_symboles[i] = ByteReading::readBytes(sector, 0x05 + i, 1);
        huffman_offset.push_back({});
    }
    buildCode(sector);
}

/**
 * Builds the huffman codes i.e. the huffman_codes hashmap and the huffman_offset vector.
 * @param sector vector of char representing a DHT section, passed by reference.
 */
void Huffman::buildCode(const std::vector<char>& sector) {
    //Huffman symbols are given from offset 0x15, sorted by increasing length.
    int count = 0;
    uint16_t code = 0;
    //Iteration over nb_symboles; which gives the number of codes given length i + 1.
    for (int i = 0; i < 16; i++){
        //Building code for each symbols
        for (int j = 1; j < nb_symboles[i] + 1; j++){
            //Hashmap
            huffman_codes[code] = (uint8_t) sector[0x15 + count];
            //Offset
            huffman_offset[i].push_back(code);
            //In order to build codes, the next code for a set length is obtained by adding 1 to the current
            //valid code.
            code++;
            count++;
        }
        //In order to build codes, when all the codes from a given length are read, the next codes have one more symbol,
        //the current valid code is shifted to the left.
        code <<= 1;
    }
}

/**
 * Returns the encoded value from a code.
 * @param code valid code in this Huffman table, given as a uint16_t.
 * @return the encoded value as a uint8_t.
 */
uint8_t Huffman::find(const uint16_t code) const{
    return huffman_codes.at(code);
}

/**
 * Test whether a symbol is a valid code.
 * @param code Symbol to be tested, as an uint16_t.
 * @param size Size of the tested symbols in int.
 * @return true if the symbol is a valid code, false otherwise.
 */
bool Huffman::contains(const uint16_t code, const int size) const {
    for (auto c : huffman_offset[size - 1]) {
       if (c == code ){
           return true;
       }
    }
    return false;
}



bool Huffman::isAC() {
    return type;
}
