//
// Created by Tom Favereau on 22/05/2024.
//

#include <valarray>
#include "JPEG.h"



    /***
     * Reads a jpeg in binary.
     * @param filename path to a jpeg file.
     * @return a char vector of all the file's bytes.
     */
     std::vector<char> JPEG::getBytes(const std::string& filename) {
        std::ifstream imageStream (filename, std::ios::binary);
        std::vector<char> imageBytes;
        if (!imageStream) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
        } else {
            char byte;
            while (imageStream.get(byte)) {
                imageBytes.push_back(byte);
            }
        }
        return imageBytes;
    }

    /***
     * From the vector of bytes of the jpeg file, returns the sectors as a vector of char vectors.
     * @param imageBytes char vector of the bytes of a jpeg file.
     * @return the sectors of a jpeg file as vector of char vectors.
     */
    std::vector<std::vector<char>> JPEG::getSectors(const std::vector<char>& imageBytes){
        std::vector<std::vector<char>> sectors;
        int i = 2; // i = 0 is the start of image marker, APP0 starts at i = 2.
        int size = ByteReading::readBytes(imageBytes, i + 2, 2); // Size offset is 2
        int nextIndex = i + size + 2; // size doesn't take into account marker
        bool endReached = false; //flag raised when reaching start of scan section (assumed to be last section)
        int marker; //used for markers
        while (!endReached) {
            sectors.push_back({imageBytes.begin() + i, imageBytes.begin() + nextIndex});
            i = nextIndex;
            marker = ByteReading::readBytes(imageBytes, i, 2);
            size = ByteReading::readBytes(imageBytes, i + 2, 2); // Size offset is 2
            nextIndex = i + size + 2;
            endReached = marker == 0xffda; //flag raising
        }
        //Push one last time
        sectors.push_back({imageBytes.begin() + i, imageBytes.begin() + nextIndex});
        //Raw data
        sectors.push_back({imageBytes.begin() + nextIndex, imageBytes.end() - 2}); // remove EOI marker
        return sectors;
    }

    JPEG::JPEG(const std::string& file_name) : file_name(file_name) {
        std::vector<std::vector<char>> sectors = getSectors(getBytes(file_name));
        for (const auto& sector : sectors) {
            int marker = ByteReading::readBytes(sector, 0, 2);
            if (marker == 0xffe0) {
                //APP0
                //TODO Check if image is in JFIF
            } else if (marker == 0xffdd) {
                //TODO DRI NOT YET SUPPORTED
                std::cerr << "DRI NOT YET SUPPORTED" << std::endl;
            } else if (marker > 0xffe0 && marker <= 0xffef) {
                //TODO APPn exception not handled
            } else if (marker == 0xffdb) {
                //DQT
                quantisationTables.push_back(quantisation_table::QuantisationTable(sector));
            } else if (marker == 0xffc0) {
                //SOF0
                precision = ByteReading::readBytes(sector, 4, 1);
                height = ByteReading::readBytes(sector, 5, 2);
                width = ByteReading::readBytes(sector, 7, 2);
                nb_comp = ByteReading::readBytes(sector, 9, 1);
                for (int i = 0x0a; i < sector.size(); i += 3) {
                    int ic = ByteReading::readBytes(sector, i, 1);
                    int ieh = ByteReading::readByte(sector[i + 1], 0, 4);
                    int iev = ByteReading::readByte(sector[i + 1], 4, 4);
                    int iq = ByteReading::readBytes(sector, i + 2, 1);
                    arrayInfoComposante.push_back(InfoComposante{.ic =  ic, .fh = ieh, .fv = iev, .iq = iq});
                }
            } else if ((marker > 0xffc0 && marker <= 0xffc3) || (marker >= 0xffc5 && marker <= 0xffc7) ) {
                //TODO Compression not handled
            } else if (marker == 0xffc4) {
                //DHT
                Huffman huff = Huffman(sector);
                if (huff.isAC()){
                    ACHuffmanTables.push_back(huff);
                } else {
                    DCHuffmanTables.push_back(huff);
                }
            } else if (marker == 0xffda) {
                //SOS
                int i = 0x05;
                int N = ByteReading::readBytes(sector, 4, 1);
                for (int j = 0; j < N; j++) {
                    int ic = ByteReading::readBytes(sector, i, 1);
                    int ihdc = ByteReading::readByte(sector[i + 1], 0, 4);
                    int ihac = ByteReading::readByte(sector[i + 1], 4, 4);
                    arrayInfoBrut.push_back(InfoBrut{.ic = ic, .ihAC = ihac, .ihDC = ihdc});
                    i += 2;
                }
            } else {
                //Raw Data
                rawData = sector;
            }
        }
    }

    uint16_t readNBits(BitReader& bitReader, int N){
        int code = 0;
        for (int i = 0; i < N; i++){
            code <<= 1;
            code += bitReader.nextBit();
        }
    }

    void addNZeroes(Block& block, int N) {
        for (int i = 0; i < N; i++){
            block.values.push_back(0);
        }
    }

    void fillZeroes(Block& block) {
        while (block.values.size() < 64) {
            block.values.push_back(0);
        }
    }

    uint16_t powerTwo(uint8_t magnitude) {
        uint16_t res = 1;
        for (int i = 0; i < magnitude; i++) {
            res = res << 1;
        }
        return res;
    }

    uint8_t decodeMagnitude(uint16_t code, uint8_t magnitude) {
        if (code > powerTwo(magnitude - 1)) {
            return code;
        } else {
            return code - (powerTwo(magnitude) - 1);
        }
    }

    Block JPEG::readBlock(const int indexDC, const int indexAC, const uint8_t& previousDC, const std::vector<char> &sector, BitReader& bitReader) const {
        const auto& huffmanDC = DCHuffmanTables[indexDC];
        const auto& huffmanAC =  ACHuffmanTables[indexAC];
        Block res;

        uint16_t code = 0;
        uint8_t magnitude;

        //-----------------DC---------------
        while (true){
            code <<= 1;
            code += bitReader.nextBit();
            if (huffmanDC.contains(code)){
                magnitude = huffmanDC.find(code);
                break;
            }
        }

        //DC Value reading (code = 0)
        code = readNBits(bitReader, magnitude);
        res.values.push_back(decodeMagnitude(code, magnitude) + previousDC);
        code = 0;

        //-------------------AC----------------------
        while(res.values.size() < 64) {
            code <<= 1;
            code += bitReader.nextBit();

            if (huffmanAC.contains(code)){
                //if code is valid
                uint8_t byte = huffmanAC.find(code);
                code = 0;
                if (byte == 0xF0) {
                    //16 zeroes
                    addNZeroes(res, 16);
                } else if (byte == 0x00) {
                    //End of block
                    fillZeroes(res);
                } else {
                    int skipZeros = ByteReading::readByte(byte, 0, 4);
                    addNZeroes(res, skipZeros);
                    magnitude = ByteReading::readByte(byte, 4, 4);
                    code = readNBits(bitReader, magnitude);
                    res.values.push_back(decodeMagnitude(code, magnitude));
                    code = 0;
                }
            }
        }
    }
 // jpeg