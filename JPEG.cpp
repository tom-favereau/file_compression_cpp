//
// Created by Tom Favereau on 22/05/2024.
//

#include "JPEG.h"
#include <fstream>
#include <iostream>

namespace jpeg {

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

    int JPEG::readBytes(const std::vector<char>& imageBytes, int index, int size){
        int res = 0;
        for (int i = 0; i < size; i++) {
            res |= (imageBytes[i + index] & 0xFF) << ((size - 1 - i) * 8);
        }
        return res;
    }

    int JPEG::nextSection(const int index, const std::vector<char>& imageBytes) {
        int size = readBytes(imageBytes, index + 2, 2);
        if (index + size < imageBytes.size()) {
            return index + size;
        } else {
            return -1;
        }
    }

    std::vector<std::vector<char>> JPEG::getSectors(const std::vector<char>& imageBytes){
        std::vector<std::vector<char>> sectors;
        int i = 2;
        int nextIndex = nextSection(i, imageBytes);
        //TODO STOP WHEN START OF SCAN IS REACHED
        while (nextIndex > -1) {
            sectors.push_back({imageBytes.begin() + i, imageBytes.begin() + nextIndex + 2});
            i = nextIndex + 2;
            nextIndex = nextSection(i, imageBytes);
        }
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