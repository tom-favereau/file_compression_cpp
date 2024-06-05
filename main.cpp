#include <iostream>
#include "JPEG.h"
int main() {

    std::string filename_a = R"(C:/Users/alici/S8_Cpp/file_compression_cpp/alexandria.jpeg)";
    std::string filename_f = R"(C:/Users/alici/S8_Cpp/file_compression_cpp/francois.jpg)";
    auto filestream = jpeg::JPEG::getBytes(filename_a);
    auto sectors = jpeg::JPEG::getSectors(filestream);
    int sector_size = 0;
    for (const auto& sector : sectors) {
        sector_size += sector.size();
    }
    std::cout << "filestream size: " << filestream.size() << std::endl;
    std::cout << "sector size: " << sector_size << std::endl;

    char c = '\010';
    std::cout << "Read Byte: " << ByteReading::readByte(c, 2, 4) << std::endl;
    jpeg::JPEG alexandria = jpeg::JPEG(filename_a);
    jpeg::JPEG francois = jpeg::JPEG(filename_f);

    std::cout << "(4, 5) : " << quantisation_table::QuantisationTable::access(4, 5) << std::endl;
    std::cout << "(7, 7) : " << quantisation_table::QuantisationTable::access(7, 7) << std::endl;
    std::cout << "(1, 6) : " << quantisation_table::QuantisationTable::access(1, 6) << std::endl;
    std::cout << "(7, 0) : " << quantisation_table::QuantisationTable::access(7, 0) << std::endl;

    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            std::cout << i << " " << j << " : " << quantisation_table::QuantisationTable::access(i, j) << std::endl;
        }
    }
}
