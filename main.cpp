#include <iostream>
#include "JPEG.h"
int main() {
    std::string filename = R"(C:\Users\alici\S8_Cpp\file_compression_cpp\francois.jpg)";
    auto filestream = jpeg::JPEG::getBytes(filename);
    for (char i: filestream)
        std::cout << i << ' ';

    auto sectors = jpeg::JPEG::getSectors(filestream);
    for (std::vector<char> i: sectors){
        std::cout << "1" << ' ';
    }
}
