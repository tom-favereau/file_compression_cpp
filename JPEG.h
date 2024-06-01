//
// Created by Tom Favereau on 22/05/2024.
//

#ifndef FILE_COMPRESSION_CPP_JPEG_H
#define FILE_COMPRESSION_CPP_JPEG_H
#include <cstdint>
#include <vector>
#include <string>
#include "ByteReading.h"
#include "QuantisationTable.h"
#include "Huffman.h"
#include <fstream>
#include <iostream>

namespace jpeg {

    struct InfoComposante {
        uint8_t ic; // indice de composante horizontal
        uint8_t fh; // facteur d'échantillonage horizontal
        uint8_t fv; // ___ vertical
        uint8_t iq; // indice quantification
    };

    struct InfoBrut{
        uint8_t ic; //indice composante
        uint8_t ihAC; //indice huffman
        uint8_t ihDC; //indice huffman
    };

    class JPEG {

    private:
        std::string file_name;
        //start of frame
        uint8_t precision;
        uint16_t height;
        uint16_t width;
        uint8_t nb_comp; //couleur ou non
        std::vector<InfoComposante> arrayInfoComposante; //tableau de taille nb_com

        //Define Quantization Tables
        std::vector<quantisation_table::QuantisationTable> quantisationTables;

        //Define Huffman Tables
        std::vector<std::vector<huffman::Huffman>> huffmanTables; // 2D!!!!!!!!!!!!!!

        //start of scan
        std::vector<InfoBrut> arrayInfoBrut; //tableau de taille n;

    public:
        JPEG(const std::string& file_name);
        static std::vector<char> getBytes(const std::string& filename);
        static std::vector<std::vector<char>> getSectors(const std::vector<char>& imageBytes);
    };

} // jpeg

#endif //FILE_COMPRESSION_CPP_JPEG_H
