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
        int start = -1;
        int end = -1;
        std::vector<int> values;
        int composante = -1; //0 Y, 1 Cb, 2 Cr
        int blockNumber = -1;
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
        std::vector<QuantisationTable> quantisationTables;

        //Define Huffman Tables
        std::vector<Huffman> DCHuffmanTables;
        std::vector<Huffman> ACHuffmanTables;

        //start of scan
        std::vector<InfoBrut> arrayInfoBrut; //tableau de taille n;

        //Raw Data
        std::vector<char> rawData;

    public:
        explicit JPEG(const std::string& file_name);

        bool readBlock(const int indexDC, const int indexAC, const uint8_t& previousDC, BitReader& bitReader, std::vector<Block>& blocks) const;

        //static methods
        static std::vector<char> getBytes(const std::string& filename);

        static std::vector<std::vector<char>> getSectors(const std::vector<char>& imageBytes);

        std::vector<Block> readBlocks();

        double InverseCosinusTransform(int x, int y, int quantisationTableIndex);

        std::vector<std::vector<std::vector<double>>> getSpatialBlocks(std::vector<Block> frequentialBlocks);
    };


#endif //FILE_COMPRESSION_CPP_JPEG_H
