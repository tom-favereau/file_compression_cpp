//
// Created by Tom Favereau on 22/05/2024.
//

#include "Huffman.h"
#include <vector>
#include "ByteReading.h"


Huffman::Huffman(const std::vector<char>& sector){
    type = ByteReading::readByte(sector[4], 0, 4);
    indice_huffman = ByteReading::readByte(sector[4], 4, 4);
    for (int i = 0; i < 16; i++){
        nb_symboles[i] = ByteReading::readBytes(sector, 0x05 + i, 1);
        huffman_offset.push_back({});
    }

    buildCode(sector);


}

void Huffman::buildCode(const std::vector<char>& sector) {
    int count = 0;
    uint16_t code = 0;
    for (int i = 0; i < 16; i++){
        for (int j = 1; j < nb_symboles[i] + 1; j++){
            huffman_codes[code] = (uint8_t) sector[0x15 + count];
            huffman_offset[i].push_back(code);
            code++;
            count++;
        }
        code <<= 1;
    }
}

uint8_t Huffman::find(const uint16_t code) const{
    return huffman_codes.at(code);
}

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
