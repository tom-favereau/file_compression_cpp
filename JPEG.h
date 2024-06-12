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
        std::vector<uint8_t> values;
        int composante = -1; //0 Y, 1 Cb, 2 Cr
        int blockNumber = -1;
    };

    struct Pixel{
        uint8_t comp1;
        uint8_t comp2;
        uint8_t comp3;
    };

    struct YCbCr{
        std::vector<std::vector<std::vector<double>>> Y;
        std::vector<std::vector<std::vector<double>>> Cb;
        std::vector<std::vector<std::vector<double>>> Cr;
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

        //colorOder
        std::unordered_map<int, int> colorOrder;

        //mcuHeights mcuWidth
        int mcuHeight;
        int mcuWidth;

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

        uint8_t InverseQuantisationCosinusTransform(int x, int y, int quantisationTableIndex, Block frequentialBlock);

        std::vector<Block> getSpatialBlocks(std::vector<Block> frequentialBlocks);

        YCbCr upscaleByComponent(std::vector<std::vector<std::vector<double>>> spatialBlocks);

        std::vector<std::vector<Pixel>> upscale(std::vector<std::vector<std::vector<double>>> spatialBlocks);

        std::vector<Block> upscaledBlock(const Block &blockToUpscale);

        static std::vector<std::vector<Pixel>> YCbCrToRGBPixels(const std::vector<std::vector<Pixel>> &pixels);

        std::vector<std::vector<Pixel>> YCbCrToPixels(YCbCr ycbcr);

        static YCbCr BlocksToYCbCr(std::vector<Block> blocks);

        //static void display(const std::vector<std::vector<Pixel>>& pixels, const std::string& filename);

        static void writePixelsToFile(const std::vector<std::vector<Pixel>>& pixels, const std::string& filename);
    };


#endif //FILE_COMPRESSION_CPP_JPEG_H
