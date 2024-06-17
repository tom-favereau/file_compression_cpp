#include <iostream>
#include <cstdlib>
#include "JPEG.h"
#include "BitReader.h"

int main() {
    //std::string filename_ft = R"(/Users/tom/Programation/dep_info/cpp/file_compression_cpp/sch.jpeg)";
    std::string filename = R"(../renee.jpeg)";

    auto filestream = JPEG::getBytes(filename);
    auto sectors = JPEG::getSectors(filestream);
    int sector_size = 0;
    for (const auto& sector : sectors) {
        sector_size += sector.size();
    }

    std::cout << "pick" << std::endl;
    JPEG image = JPEG(filename);
    auto blocks = image.readBlocks();
    std::vector<Block> spatialBlock = image.getSpatialBlocks(blocks);
    YCbCr YCbCr_francois = JPEG::BlocksToYCbCr(spatialBlock);
    std::vector<std::vector<Pixel>> pixels_francois = image.YCbCrToPixels(YCbCr_francois);
    std::vector<std::vector<Pixel>> rgb_pixels_francois = JPEG::YCbCrToRGBPixels(pixels_francois);
    //JPEG::writePixelsToFile(pixels_francois, "francois_test");
    JPEG::writePixelsToFile(rgb_pixels_francois, "../image.out");
    int result = system("python3 ../display.py ../image.out");

    if (result == 0) {
        std::cout << "exit success" << std::endl;
    } else {
        std::cerr << "failed with code " << result << std::endl;
    }


}
