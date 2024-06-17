#include <iostream>
#include <cstdlib>
#include "JPEG.h"
#include "BitReader.h"

int main() {
    std::string filename = R"(../renee.jpeg)";

    auto filestream = JPEG::getBytes(filename);
    auto sectors = JPEG::getSectors(filestream);
    int sector_size = 0;
    for (const auto& sector : sectors) {
        sector_size += sector.size();
    }

    JPEG image = JPEG(filename);
    auto blocks = image.readBlocks();
    std::vector<Block> spatialBlock = image.getSpatialBlocks(blocks);
    YCbCr YCbCr_ = JPEG::BlocksToYCbCr(spatialBlock);
    std::vector<std::vector<Pixel>> pixels = image.YCbCrToPixels(YCbCr_);
    std::vector<std::vector<Pixel>> rgb_pixels= JPEG::YCbCrToRGBPixels(pixels);
    JPEG::writePixelsToFile(rgb_pixels, "../image.out");
    int result = system("python3 ../display.py ../image.out");

    if (result == 0) {
        std::cout << "exit success" << std::endl;
    } else {
        std::cerr << "failed with code " << result << std::endl;
    }


}
