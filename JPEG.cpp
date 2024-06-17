//
// Created by Tom Favereau on 22/05/2024.
//

#include <valarray>
#include <cmath>
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

    /**
     * Constructs a JPEG instance from a JPEG image.
     * Currently, only JFIF images with no restart intervals nor subsampling are handled. Images
     * which use a single DHT or DQT section to define multiple huffman table or quantisation tables are
     * not handled either.
     * @param file_name in string.
     */
    JPEG::JPEG(const std::string& file_name) : file_name(file_name) {
        std::vector<std::vector<char>> sectors = getSectors(getBytes(file_name));
        //Identifying sectors
        for (const auto& sector : sectors) {
            int marker = ByteReading::readBytes(sector, 0, 2);
            if (marker == 0xffe0) {
                //APP0
                std::cout << "APP0 DETECTED" << std::endl;
                //TODO Check if image is in JFIF
            } else if (marker == 0xffdd) {
                //TODO DRI NOT YET SUPPORTED
                std::cerr << "DRI NOT YET SUPPORTED" << std::endl;
            } else if (marker > 0xffe0 && marker <= 0xffef) {
                //TODO APPn exception not handled
                std::cerr << "APPn not handled" << std::endl;
                std::cerr << (int) marker << std::endl;
            } else if (marker == 0xffdb) {
                //DQT
                std::cout << "DQT DETECTED" << std::endl;
                quantisationTables.push_back(QuantisationTable(sector));
            } else if (marker == 0xffc0) {
                //SOF0
                std::cout << "SOF0 DETECTED" << std::endl;
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
                std::cerr << "Compression not handled" << std::endl;
                std::cerr << (int) marker << std::endl;
            } else if (marker == 0xffc4) {
                //DHT
                std::cout << "DHT DETECTED" << std::endl;
                Huffman huff = Huffman(sector);
                if (huff.isAC()){
                    ACHuffmanTables.push_back(huff);
                } else {
                    DCHuffmanTables.push_back(huff);
                }
            } else if (marker == 0xFFFE) {
                std::cout << "COM DETECTED" << std::endl;
            } else if (marker == 0xffda) {
                //SOS
                std::cout << "SOS DETECTED" << std::endl;
                int i = 0x05;
                int N = ByteReading::readBytes(sector, 4, 1);
                for (int j = 0; j < N; j++) {
                    int ic = ByteReading::readBytes(sector, i, 1);
                    int ihdc = ByteReading::readByte(sector[i + 1], 0, 4);
                    int ihac = ByteReading::readByte(sector[i + 1], 4, 4);
                    arrayInfoBrut.push_back(InfoBrut{.ic = ic, .ihAC = ihac, .ihDC = ihdc});
                    i += 2;
                }
            } else if (((marker >> 8) & 0xFF) == 0xFF) {
                std::cerr << "Unknown marker " << (int) marker << std::endl;
            }
            else {
                //Raw Data
                rawData = sector;
            }
        }
        mcuHeight = (height + 7) / 8;
        mcuWidth = (width + 7) / 8;

        //UNUSED LINES BECAUSE SUBSAMPLING IS NOT HANDLED
        if (mcuHeight % 2 && arrayInfoComposante[0].fv == 2) {
            mcuHeight++;
        }


        if (mcuWidth % 2 && arrayInfoComposante[0].fh == 2) {
            mcuWidth++;
        }


        //Initialization of colorOrder
        for (int i = 0; i < nb_comp; i++) {
            int indexInSOS;
            for (int j = 0; j < nb_comp; j++){
                if (arrayInfoComposante[j].ic == arrayInfoBrut[i].ic) {
                    indexInSOS = j;
                }
            }
            colorOrder[i] = indexInSOS;
        }
    }

    /**
     * Adds N zeroes to the values of a block.
     * @param block in which N zeroes will be added.
     * @param N int, number of zeroes to add.
     */
    void addNZeroes(Block& block, int N) {
        for (int i = 0; i < N; i++){

            block.values.push_back(0);

        }
    }

    /**
     * Fills up the remaining space of a block with zeroes (up to 64 because a block is 8x8).
     * @param block to fill with zeroes.
     */
    void fillZeroes(Block& block) {
        while (block.values.size() < 64) {
            block.values.push_back(0);
        }
    }

    /**
     * Calculates 2 to the power magnitude.
     * @param magnitude int, power.
     * @return 2^magnitude as an uint16_t.
     */
    uint16_t powerTwo(uint16_t magnitude) {
        return 1 << magnitude;
    }

    /**
     * Decodes a encoded value by entropy encoding.
     * @param code code to be decoded, uint16_t.
     * @param magnitude magnitude (size of code) in uint16_t (at most eleven).
     * @return the value of the entropy code.
     */
    int decodeMagnitude(uint16_t code, uint16_t magnitude) {
        int res;
        //If the code starts by one, i.e. if code >= 2^(magnitude - 1), the code is the value
        if (code >= powerTwo(magnitude - 1)) {
            res = code;
        } else {
            //Otherwise the value is code - 2^magnitude + 1
            //Inverting 0 to 1 and vice versa also works.
            res = code - (powerTwo(magnitude) - 1);
        }
        return res;
    }

    /**
     * Reads the next block from the huffman encoded byte stream.
     * @param indexDC Index of the DC Huffman table to be used.
     * @param indexAC Index of the AC Huffman table to be used.
     * @param previousDC Value of previous DC for a given color.
     * @param bitReader Reference to a bit reader reading the huffman encoded byte stream.
     * @param blocks reference to a vector of blocks where the results of block reading are stored.
     * @return true if a block was successfully read, false otherwise.
     */
    bool JPEG::readBlock(const int indexDC, const int indexAC, const int64_t& previousDC, BitReader& bitReader, std::vector<Block>& blocks) const {
        Block res;

        //std::cout << "previousDC" << previousDC << std::endl;
        //Shouldn't happen
        if (!bitReader.hasNextBit()) {
            std::cerr << "BR NO MORE" << std::endl;
            return false;
        }
        //Load huffman tables
        const auto& huffmanDC = DCHuffmanTables[indexDC];
        const auto& huffmanAC =  ACHuffmanTables[indexAC];

        //Code reading is done with code
        uint16_t code = 0;
        //Stores magnitude
        int magnitude;
        //Stores size of the code.
        int size = 0;

        //-----------------DC---------------
        //DC values are stored first. They are encoded differentially, previous DC values are thus needed.
        //DC values are decoded by first decoding the magnitude of the differential DC value.....
        while (bitReader.hasNextBit()){
            code <<= 1;
            code += bitReader.nextBit();
            size++;
            if (huffmanDC.contains(code, size)){
                magnitude = huffmanDC.find(code);
                size = 0;
                break;
            }
        }

        //DC Value reading (code = 0)
        //........then, by reading the entropy encoded differential DC value.
        code = bitReader.nextNBits(magnitude);
        res.values.push_back(decodeMagnitude(code, magnitude) + previousDC);
        code = 0;

        //-------------------AC----------------------
        //Next, there are AC 63 values.
        //162 bytes are encoded in an AC huffman table:
        //Normal bytes: 4 bits for number of zero values (thus up to 15) between previous and current non-zero values
        //4 bits for current AC value
        //Two special bytes: skip more than 16 zeroes (0xF0) and end of block (0x00) (all remaining values are 0).
        while(res.values.size() < 64 && bitReader.hasNextBit()) {
            code <<= 1;
            code += bitReader.nextBit();
            size++;
            if (huffmanAC.contains(code, size)){
                //if code is valid
                uint8_t byte = huffmanAC.find(code);
                code = 0;
                size = 0;
                if (byte == 0xF0) {
                    //16 zeroes
                    addNZeroes(res, 16);
                } else if (byte == 0x00) {
                    //End of block
                    fillZeroes(res);
                } else {
                    int skipZeros = ByteReading::readByte(byte, 0, 4);
                    addNZeroes(res, skipZeros);
                    magnitude = ByteReading::readByte(byte, 4, 4);
                    code = bitReader.nextNBits(magnitude);
                    res.values.push_back(decodeMagnitude(code, magnitude));

                    code = 0;
                }
            }
        }
        //For debugging purposes
        if (res.values.size() != 64) {
            std::cerr << "BLOCK IS " << res.values.size() << " LONG" << std::endl;
            std::cerr << "Current Sector index: " << bitReader.getCurrentSectorIndex() << std::endl;
            std::cerr << "Current Byte index: " << bitReader.getCurrentByteIndex() << std::endl;
            std::cerr << blocks.size() << " blocks." << std::endl;
            return false;
        }
        blocks.push_back(res);
        return true;
    }

    /**
     * Read all the blocks from the huffman encoded data of this JPEG image.
     * @return a vector of all the blocks encoded in this JPEG image.
     */
    std::vector<Block> JPEG::readBlocks() {
        //Initialization of bit reading.
        BitReader br = BitReader(rawData);
        std::vector<Block> blocks;


        //DC values are initialized for all colors to 0.
        int16_t previousDC[3] = {0};
        //Total number of blocks
        int count = 0;
        //Iteration over the number of MCUs
        for (int i = 0; i < mcuWidth * mcuHeight; i++) {
            //Iteration over the number of component (grayscale: 1, color: 3 (YCbCr))
            for (int j = 0; j < nb_comp; j++) {
                //Iteration over the number of Blocks require to form a full MCU if Cb and Cr are subsampling.
                for (int k = 0; k < arrayInfoComposante[colorOrder[j]].fh * arrayInfoComposante[colorOrder[j]].fv; k++) {
                    //Loop bool for debugging purposes and ensure a block is safely read.
                    bool loop = readBlock(arrayInfoBrut[j].ihDC, arrayInfoBrut[j].ihAC, previousDC[j], br, blocks);
                    if (loop) {
                        previousDC[j] = (int16_t) blocks[blocks.size() - 1].values[0];
                        blocks[blocks.size() - 1].blockNumber = count;
                        count++;
                        blocks[blocks.size() - 1].composante = j;
                    } else {
                        std::cerr << "READING INTERRUPTED" << std::endl;
                        std::cerr << "Current Sector index: " << br.getCurrentSectorIndex() << std::endl;
                        std::cerr << "Current Byte index: " << br.getCurrentByteIndex() << std::endl;
                        std::cerr << blocks.size() << " blocks." << std::endl;
                        std::cerr << "Composante " << j << ", " << "k = " << k << std::endl;
                        break;
                    }
                }
            }
        }
        //std::vector<Block> last10Blocks = {blocks.end() - 10, blocks.end()};
        //Debugging purposes: image that are not correctly decompressed usually have remaining information.
        std::cout << "Last Byte Read: " << br.getCurrentByte() << " is byte " << br.getCurrentSectorIndex() << " out of " << br.getSectorSize() - 1 << std::endl;
        std::cout << "Last Byte index " << br.getCurrentByteIndex() << std::endl;
        std::cout << blocks.size() << " blocks." << std::endl;
        return blocks;
    }

    /**
     * Applies Inverse Quantization and Inverse Cosinus Transform to a spatial pixel.
     * @param x Abscissa of the spatial pixel in the 8x8 spatial block.
     * @param y Ordinate of the spatial pixel in the 8x8 spatial block.
     * @param quantisationTableIndex index of the quantisation table to be used for the frequential block.
     * @param frequentialBlock frequential block with 64 frequential values store in zig zag order.
     * @return the computed value at (x,y) in the spatial block.
     */
    int JPEG::InverseQuantisationCosinusTransform(int x, int y, int quantisationTableIndex, Block frequentialBlock){
        double sum = 0;
        double Clambda = 0;
        double Cmu = 0;
        double pi = 4*atan(1);
        for (int lambda = 0; lambda < 8; lambda++){
            for (int mu = 0; mu < 8; mu++){
                if (mu == 0){
                    Cmu = 1.0/ sqrt(2.0);
                } else {
                    Cmu = 1.0;
                }
                if (lambda == 0){
                    Clambda = 1.0/ sqrt(2.0);
                } else {
                    Clambda = 1.0;
                }

                sum += Cmu * Clambda * cos( (2.0 * x + 1.0) * lambda * pi / 16.0) * cos((2.0 * y + 1.0) * mu * pi / 16.0) *
                        quantisationTables[quantisationTableIndex].elementAt(lambda, mu) *
                        frequentialBlock.values[QuantisationTable::access(lambda, mu)];
            }
        }
        return (1.0/4.0 * sum);
    }

    /**
     * Converts frequential blocks to spatial blocks.
     * @param frequentialBlocks blocks to be transformed.
     * @return vector of spatial blocks obtained by transformation.
     */
    std::vector<Block> JPEG::getSpatialBlocks(std::vector<Block> frequentialBlocks){

        std::vector<Block> spatialBlocks;
        for (int i = 0; i < frequentialBlocks.size(); i++) {
            std::vector<int> values;
            Block test = frequentialBlocks[i];
            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {
                    values.push_back(InverseQuantisationCosinusTransform(x, y,
                                                                         arrayInfoComposante[frequentialBlocks[i].composante].iq,
                                                                         frequentialBlocks[i]));
                }
            }
            spatialBlocks.push_back(Block{});
            spatialBlocks[i].values = values;
            spatialBlocks[i].composante = frequentialBlocks[i].composante;
        }
        return spatialBlocks;
    }

    //UNFINISHED
    std::vector<Block> JPEG::upscaledBlock(const Block& blockToUpscale) {
        std::vector<Block> res;
        //only supports JFIF
        if (blockToUpscale.composante != arrayInfoComposante[0].ic) {

        } else {
            res.push_back(blockToUpscale);
        }
        return res;
    }

    //UNFINISHED
    YCbCr JPEG::upscaleByComponent(std::vector<std::vector<std::vector<double>>> spatialBlocks){
    }

//UNFINISHED
std::vector<std::vector<Pixel>> JPEG::upscale(std::vector<std::vector<std::vector<double>>> spatialBlocks) {
    std::vector<std::vector<Pixel>> imageYCC;
    for (int i = 0; i < height; i++){
        std::vector<Pixel> ligne(width);
        imageYCC.push_back(ligne);
    }



    int count = 0;
    for (int i = 0; i < mcuWidth * mcuHeight; i++) {
        for (int j = 0; j < nb_comp; j++) {
            for (int k = 0; k < arrayInfoComposante[colorOrder[j]].fh; k++) {
                for (int l = 0; l < arrayInfoComposante[colorOrder[j]].fv; l++) {

                }

            }
        }
    }
    return imageYCC;
}

/**
 * Converts a YCbCr pixel to a RGB pixel
 * @param pixel YCbCr pixel with values obtained from the inverse quantisation and cosinus transform.
 * Y values range from -128 to 127.
 * @return the pixel in RGB color coordinates.
 */
Pixel YCbCrToRGB(Pixel pixel) {
        Pixel res{};

        int r = pixel.comp1 + 1.402f * pixel.comp3 + 128;
        int g = pixel.comp1 - 0.344f * pixel.comp2 - 0.714f * pixel.comp3 + 128;
        int b = pixel.comp1 + 1.772f * pixel.comp2 + 128;
        if (r < 0)   r = 0;
        if (r > 255) r = 255;
        if (g < 0)   g = 0;
        if (g > 255) g = 255;
        if (b < 0)   b = 0;
        if (b > 255) b = 255;
        res.comp1 = r;
        res.comp2 = g;
        res.comp3 = b;

        //res.comp1 = pixel.comp1 - 0.0009267 * (pixel.comp2 - 128) + 1.4016868 * (pixel.comp3 - 128);
        //res.comp2 = pixel.comp1 - 0.3436954 * (pixel.comp2 - 128) - 0.7141690 * (pixel.comp3 - 128);
        //res.comp3 = pixel.comp1 + 1.7721604 * (pixel.comp2 - 128) + 0.0009902 * (pixel.comp3 - 128);
        return res;
    }

    /**
     * Converts YCbCr pixels of a JPEG image to RGB pixels.
     * @param pixels YCbCr pixels of a JPEG image given as an vector of vector of pixels.
     * @return pixels of the JPEG image in RGB color coordinates as an vector of vector of pixels.
     */
std::vector<std::vector<Pixel>> JPEG::YCbCrToRGBPixels(const std::vector<std::vector<Pixel>>& pixels) {
        std::vector<std::vector<Pixel>> res;
        for (int i = 0; i < pixels.size(); i++) {
            res.push_back(std::vector<Pixel> {});
        }

        for (int i = 0; i < pixels.size(); i++) {
            for (int j = 0; j < pixels[0].size(); j++) {
                res[i].push_back(YCbCrToRGB(pixels[i][j]));
            }
        }

        return res;
    }

    /**
     * Converts a vector of blocks to a YCbCr structure.
     * @param blocks Blocks to be converted to YCbCr
     * @return vectors of Y, Cb, Cr component store in a YCbCr structure.
     */
YCbCr JPEG::BlocksToYCbCr(std::vector<Block> blocks) {
        YCbCr ycbcr;
        for (auto & block : blocks) {
            std::vector<std::vector<double>> doubleBlock;
            for (int x = 0; x < 8; x++) {
                doubleBlock.push_back({});
                for (int y = 0; y < 8; y++) {
                    doubleBlock[x].push_back(block.values[x*8+y]);
                }
            }
            //Here colorOrder was ignored, it works most of the times.
            if (block.composante == 0) {
                ycbcr.Y.push_back(doubleBlock);
            } else if (block.composante == 1) {
                ycbcr.Cb.push_back(doubleBlock);
            } else if (block.composante == 2) {
                ycbcr.Cr.push_back(doubleBlock);
            } else {
                std::cerr << "Unknown component in BlocksToYCbCr." << std::endl;
            }
        }
        return ycbcr;
    }

    /**
     * Converts the 8x8 Blocks of Y, Cb, Cr components of this image to height * width vector of vector of pixels.
     * @param ycbcr YCbCr structure containing all 8x8 blocks of this image.
     * @return a vector of vector of pixels in YCbCr of dimensions height*width.
     * DOES NOT SUPPORT UPSCALING YET
     */
std::vector<std::vector<Pixel>> JPEG::YCbCrToPixels(YCbCr ycbcr) const {
        //SANS UPSCALE
        std::vector<std::vector<Pixel>> res;
        for (int i = 0; i < height; i++) {
            std::vector<Pixel> line;
            for (int j = 0; j < width; j++){
                line.push_back(Pixel{});
            }
            res.push_back(line);
        }

        for (int k = 0; k < ycbcr.Y.size(); k++) {
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    int remainder = width%8;
                    int x;
                    int y;
                    if (remainder != 0) {
                        x = 8 * (k * 8 / (width + 8 - remainder)) + i;
                        y = (k * 8 % (width + 8 - remainder)) + j;
                    } else {
                        x = 8 * (k * 8 / width) + i;
                        y = (k * 8 % width ) + j;
                    }
                    if (x < height && y < width) {
                        res[x][y].comp1 = ycbcr.Y[k][i][j];
                        res[x][y].comp2 = ycbcr.Cb[k][i][j];
                        res[x][y].comp3 = ycbcr.Cr[k][i][j];

                    }
                }
            }
        }
        return res;
    }

/**
 * Writes pixels to a file used with display.py.
 * @param pixels vector of vector of the pixels of a JPEG image.
 * @param filename name of file to write in.
 */
void JPEG::writePixelsToFile(const std::vector<std::vector<Pixel>>& pixels, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Erreur d'ouverture du fichier !" << std::endl;
        return;
    }
    int height = pixels.size();
    int width = pixels[0].size();
    file << height << " " << width << std::endl;  // Écrire les dimensions de l'image
    for (const auto& row : pixels) {
        for (const auto& pixel : row) {
            file << (int) pixel.comp1 << " " << (int) pixel.comp2 << " " << (int) pixel.comp3 << " ";
        }
        file << std::endl;
    }
    file.close();
}

//void JPEG::display(const std::vector<std::vector<Pixel>>& pixels, const std::string& filename) {
//    writePixelsToFile(pixels, filename);
//    std::string tmp = "python3 display.py" + filename;
//    int result = std::system(tmp);

//}

// jpeg