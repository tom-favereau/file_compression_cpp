#include <iostream>
#include "JPEG.h"
#include "BitReader.h"

int main() {

    std::string filename_a = R"(C:/Users/alici/S8_Cpp/file_compression_cpp/alexandria.jpeg)";
    std::string filename_f = R"(C:/Users/alici/S8_Cpp/file_compression_cpp/francois.jpg)";
    std::string filename_c = R"(C:/Users/alici/S8_Cpp/file_compression_cpp/cat.jpg)";
    std::string filename_g = R"(C:/Users/alici/S8_Cpp/file_compression_cpp/goldfish_2to1H.jpg)";
    std::string filename_t = R"(C:/Users/alici/S8_Cpp/file_compression_cpp/test.jpeg)";
    std::string filename_p = R"(C:/Users/alici/S8_Cpp/file_compression_cpp/pick.jpeg)";

    auto filestream = JPEG::getBytes(filename_a);
    auto sectors = JPEG::getSectors(filestream);
    int sector_size = 0;
    for (const auto& sector : sectors) {
        sector_size += sector.size();
    }
    std::cout << "filestream size: " << filestream.size() << std::endl;
    std::cout << "sector size: " << sector_size << std::endl;

    char c = '\010';
    std::cout << "Read Byte: " << ByteReading::readByte(c, 2, 4) << std::endl;
    std::cout << "(4, 5) : " << quantisation_table::QuantisationTable::access(4, 5) << std::endl;
    std::cout << "(7, 7) : " << quantisation_table::QuantisationTable::access(7, 7) << std::endl;
    std::cout << "(1, 6) : " << quantisation_table::QuantisationTable::access(1, 6) << std::endl;
    std::cout << "(7, 0) : " << quantisation_table::QuantisationTable::access(7, 0) << std::endl;

    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            std::cout << i << " " << j << " : " << quantisation_table::QuantisationTable::access(i, j) << std::endl;
        }
    }

    //std::cout << "Alexandria" << std::endl; // 2 quantization 1 DQT
    //JPEG alexandria = JPEG(filename_a);
    std::cout << "Francois" << std::endl;
    JPEG francois = JPEG(filename_f);
    auto francois_Blocks = francois.readBlocks();
    //std::cout << "Cat" << std::endl; // 4 Huffman 1 DHT
    //JPEG cat = JPEG(filename_c);
    //auto cat_Blocks = cat.readBlocks();
    //std::cout << "goldfish" << std::endl;
    //JPEG goldfish = JPEG(filename_g);
    //auto gold_Blocks = goldfish.readBlocks();
    std::cout << "pick" << std::endl;
    JPEG pick = JPEG(filename_p);
    auto pick_Blocks = pick.readBlocks();
    std::cout << "test" << std::endl;
    JPEG test = JPEG(filename_t);
    auto test_Blocks = test.readBlocks();
}
