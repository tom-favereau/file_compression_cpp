//
// Created by Tom Favereau on 22/05/2024.
//

#include "Huffman.h"
#include <vector>
#include "ByteReading.h"


Huffman::Huffman(const std::vector<char>& sector){
    bool bit0 = sector[4] & 0b00000001;  // Vérifie le bit 0
    bool bit1 = sector[4] & 0b00000010;  // Vérifie le bit 1
    bool bit2 = sector[4] & 0b00000100;  // Vérifie le bit 2
    bool bit3 = sector[4] & 0b00001000;  // Vérifie le bit 3
    bool bit4 = sector[4] & 0b00010000;  // Vérifie le bit 4

    if (bit0){
        indice_huffman = 0;
    } else if (bit1){
        indice_huffman = 1;
    } else if (bit2){
        indice_huffman = 2;
    } else if (bit3){
        indice_huffman = 3;
    }

    type = bit4;

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
