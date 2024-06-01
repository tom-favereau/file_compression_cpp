//
// Created by Tom Favereau on 22/05/2024.
//

#include "JPEG.h"

namespace jpeg {

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
        sectors.push_back({imageBytes.begin() + nextIndex, imageBytes.end()});
        return sectors;
    }

    JPEG::JPEG(const std::string& file_name) : file_name(file_name) {
            std::vector<std::vector<char>> sectors = getSectors(getBytes(file_name));
            for (const auto& sector : sectors) {
                int marker = ByteReading::readBytes(sector, 0, 2);
                if (marker == 0xffe0) {
                    //APP0
                    //TODO Check if image is in JFIF
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
                        //SOF0
                        int ic = ByteReading::readBytes(sector, i, 1);
                        int ieh = ByteReading::readByte(sector[i + 1], 0, 4);
                        int iev = ByteReading::readByte(sector[i + 1], 4, 4);
                        int iq = ByteReading::readBytes(sector, i + 2, 1);
                        arrayInfoComposante.push_back(InfoComposante(ic, ieh, iev, iq));
                    }
                } else if (marker == 0xffc4) {
                    //DHT
                    //huffmanTables[0].push_back(Huffman(sector));
                } else if (marker == 0xffda) {
                    //SOS
                } else {
                    //Raw Data
                }
            }
        }


}
 // jpeg