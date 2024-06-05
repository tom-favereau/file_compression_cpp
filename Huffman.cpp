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
        nb_symboles[i] = ByteReading::readBytes(sector, 0x05+i, 1);
    }

    buildCode(sector);


}

void Huffman::buildCode(const std::vector<char>& sector) {
    int count = 0;
    uint16_t code = 0;
    for (int nb : nb_symboles){
        for (int j = 0; j < nb; j++){
            huffman_codes[(uint8_t) sector[21+count]] = code;
            code++;
            count++;
        }
        code <<= 1;
    }
}

std::vector<uint8_t> Huffman::readBlock(const uint8_t& previousDC, const std::vector<char> &sector) {
    
}

bool Huffman::isAC() {
    return type;
}
