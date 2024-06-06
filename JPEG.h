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
#include "BitReader.h"



    struct InfoComposante {
        int ic; // indice de composante horizontal
        int fh; // facteur d'échantillonage horizontal
        int fv; // ___ vertical
        int iq; // indice quantification
    };

    struct InfoBrut{
        int ic; //indice composante
        int ihAC; //indice huffman
        int ihDC; //indice huffman
    };

    struct Block{
        int start;
        int end;
        std::vector<uint8_t> values;
        int composante; //0 Y, 1 Cb, 2 Cr
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
        std::vector<Huffman> DCHuffmanTables;
        std::vector<Huffman> ACHuffmanTables;

        //start of scan
        std::vector<InfoBrut> arrayInfoBrut; //tableau de taille n;

        //Raw Data
        std::vector<char> rawData;

    public:
        explicit JPEG(const std::string& file_name);

        Block readBlock(const int indexDC, const int indexAC, const uint8_t& previousDC, const std::vector<char>& sector, BitReader& bitReader) const;

        //static methods
        static std::vector<char> getBytes(const std::string& filename);

        static std::vector<std::vector<char>> getSectors(const std::vector<char>& imageBytes);
    };


#endif //FILE_COMPRESSION_CPP_JPEG_H
