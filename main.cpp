#include <iostream>
#include "JPEG.h"
int main() {

    std::string filename = R"(C:\Users\alici\S8_Cpp\file_compression_cpp\alexandria.jpeg)";
    auto filestream = jpeg::JPEG::getBytes(filename);
    auto sectors = jpeg::JPEG::getSectors(filestream);
    int sector_size = 0;
    for (const auto& sector : sectors) {
        sector_size += sector.size();
    }
    std::cout << "filestream size: " << filestream.size() << std::endl;
    std::cout << "sector size: " << sector_size << std::endl;
}
