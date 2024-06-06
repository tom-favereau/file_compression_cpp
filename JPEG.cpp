//
// Created by Tom Favereau on 22/05/2024.
//

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

    Block JPEG::readBlock(const int indexDC, const int indexAC, const uint8_t& previousDC, const std::vector<char> &sector, BitReader& bitReader) const {
        Huffman huffmanDC = DCHuffmanTables[indexDC];
        Huffman huffmanAC =  ACHuffmanTables[indexAC];
        Block res;

        uint16_t code = 0;
        while (true){
            code <<= 1;
            code += bitReader.nextBit();
            if (huffmanDC.contains(code)){
                res.values.push_back(huffmanDC.find(code));
            }
        }

        for (int i = 0; i < 63; i++){

        }


    }

 // jpeg