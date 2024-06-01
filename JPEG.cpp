//
// Created by Tom Favereau on 22/05/2024.
//

#include "JPEG.h"
#include <fstream>
#include <iostream>

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

    /**
     * Read bytes in a char vector and converts them to an int.
     * @param imageBytes char vector which is read.
     * @param index index at which reading starts.
     * @param size number of bytes read to convert to an int.
     * @return the int represented by the bytes starting at index index of size size.
     */
    int JPEG::readBytes(const std::vector<char>& imageBytes, int index, int size){
        int res = 0;
        for (int i = 0; i < size; i++) {
            res |= (imageBytes[i + index] & 0xFF) << ((size - 1 - i) * 8);
        }
        return res;
    }

    /***
     * From the vector of bytes of the jpeg file, returns the sectors as a vector of char vectors.
     * @param imageBytes char vector of the bytes of a jpeg file.
     * @return the sectors of a jpeg file as vector of char vectors.
     */
    std::vector<std::vector<char>> JPEG::getSectors(const std::vector<char>& imageBytes){
        std::vector<std::vector<char>> sectors;
        int i = 2; // i = 0 is the start of image marker, APP0 starts at i = 2.
        int size = readBytes(imageBytes, i + 2, 2); // Size offset is 2
        int nextIndex = i + size + 2;
        //TODO STOP WHEN START OF SCAN IS REACHED
        bool endReached = false;
        while (!endReached) {
            sectors.push_back({imageBytes.begin() + i, imageBytes.begin() + nextIndex});
            i = nextIndex;
            int marker = readBytes(imageBytes, i, 2);
            size = readBytes(imageBytes, i + 2, 2); // Size offset is 2
            nextIndex = i + size + 2;
            endReached = marker == 0xffda;
        }
        sectors.push_back({imageBytes.begin() + i, imageBytes.begin() + nextIndex});
        sectors.push_back({imageBytes.begin() + nextIndex, imageBytes.end()});
        return sectors;
    }

    JPEG::JPEG(const std::string& file_name) : file_name(file_name) {
            //Check if image is in JFIF
            //Check if SOFx is SOFO i.e. OxFFC0
            //Get precision
            //Get height
            //Get width
            //Get nb_comp
            //Get InfoComposante
            //Get Quantization table
        }


}
 // jpeg