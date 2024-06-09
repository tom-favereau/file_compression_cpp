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

    JPEG::JPEG(const std::string& file_name) : file_name(file_name) {
        std::vector<std::vector<char>> sectors = getSectors(getBytes(file_name));
        for (const auto& sector : sectors) {
            int marker = ByteReading::readBytes(sector, 0, 2);
            if (marker == 0xffe0) {
                //APP0
                //TODO Check if image is in JFIF
            } else if (marker == 0xffdd) {
                //TODO DRI NOT YET SUPPORTED
                std::cerr << "DRI NOT YET SUPPORTED" << std::endl;
            } else if (marker > 0xffe0 && marker <= 0xffef) {
                //TODO APPn exception not handled
                std::cerr << "APPn not handled" << std::endl;
            } else if (marker == 0xffdb) {
                //DQT
                quantisationTables.push_back(QuantisationTable(sector));
            } else if (marker == 0xffc0) {
                //SOF0
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
            } else if (marker == 0xffc4) {
                //DHT
                Huffman huff = Huffman(sector);
                if (huff.isAC()){
                    ACHuffmanTables.push_back(huff);
                } else {
                    DCHuffmanTables.push_back(huff);
                }
            } else if (marker == 0xffda) {
                //SOS
                int i = 0x05;
                int N = ByteReading::readBytes(sector, 4, 1);
                for (int j = 0; j < N; j++) {
                    int ic = ByteReading::readBytes(sector, i, 1);
                    int ihdc = ByteReading::readByte(sector[i + 1], 0, 4);
                    int ihac = ByteReading::readByte(sector[i + 1], 4, 4);
                    arrayInfoBrut.push_back(InfoBrut{.ic = ic, .ihAC = ihac, .ihDC = ihdc});
                    i += 2;
                }
            } else {
                //Raw Data
                rawData = sector;
            }
        }
        mcuHeight = (height + 7) / 8;
        mcuWidth = (width + 7) / 8;

        if (mcuHeight % 2 && arrayInfoComposante[0].fv == 2) {
            mcuHeight++;
        }


        if (mcuWidth % 2 && arrayInfoComposante[0].fh == 2) {
            mcuWidth++;
        }




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

    void addNZeroes(Block& block, int N) {
        for (int i = 0; i < N; i++){
            block.values.push_back(0);
        }
    }

    void fillZeroes(Block& block) {
        while (block.values.size() < 64) {
            block.values.push_back(0);
        }
    }

    uint16_t powerTwo(uint8_t magnitude) {
        return 1 << magnitude;
    }

    int decodeMagnitude(uint16_t code, uint8_t magnitude) {
        int res;
        if (code >= powerTwo(magnitude - 1)) {
            res = code;
        } else {
            res = code - (powerTwo(magnitude) - 1);
        }
        return res;
    }

    bool JPEG::readBlock(const int indexDC, const int indexAC, const uint8_t& previousDC, BitReader& bitReader, std::vector<Block>& blocks) const {
        Block res;
        if (!bitReader.hasNextBit()) {
            std::cerr << "BR NO MORE" << std::endl;
            return false;
        }
        const auto& huffmanDC = DCHuffmanTables[indexDC];
        const auto& huffmanAC =  ACHuffmanTables[indexAC];


        uint16_t code = 0;
        uint8_t magnitude;
        int size = 0;

        //-----------------DC---------------
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
        code = bitReader.nextNBits(magnitude);
        res.values.push_back(decodeMagnitude(code, magnitude) + previousDC);
        code = 0;

        //-------------------AC----------------------
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

    std::vector<Block> JPEG::readBlocks() {
        BitReader br = BitReader(rawData);
        std::vector<Block> blocks;



        int previousDC[3] = {0};
        int count = 0;
        for (int i = 0; i < mcuWidth * mcuHeight; i++) {
            for (int j = 0; j < nb_comp; j++) {
                for (int k = 0; k < arrayInfoComposante[colorOrder[j]].fh * arrayInfoComposante[colorOrder[j]].fv; k++) {
                    bool loop = readBlock(arrayInfoBrut[j].ihDC, arrayInfoBrut[j].ihAC, previousDC[j], br, blocks);
                    if (loop) {
                        previousDC[j] = blocks[blocks.size() - 1].values[0];
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
        std::cout << "Last Byte " << br.getCurrentByte() << std::endl;
        std::cout << "Last Byte index " << br.getCurrentByteIndex() << std::endl;
        return blocks;
    }

    double JPEG::InverseQuantisationCosinusTransform(int x, int y, int quantisationTableIndex, Block frequentialBlock){
        double sum = 0;
        double Clambda = 0;
        double Cmu = 0;
        double pi = 4*atan(1);
        for (int lambda = 0; lambda < 8; lambda++){
            for (int mu = 0; mu < 8; mu++){
                if (mu == 0){
                    Cmu = 1/ sqrt(2);
                } else {
                    Cmu = 1;
                }
                if (lambda == 0){
                    Clambda = 1/ sqrt(2);
                } else {
                    Clambda = 1;
                }

                sum += Cmu * Clambda * cos( (2 * x + 1) * lambda * pi / 16) * cos((2 * y + 1) * mu * pi / 16) *
                        quantisationTables[quantisationTableIndex].elementAt(lambda, mu) *
                        frequentialBlock.values[QuantisationTable::access(lambda, mu)];
            }
        }
        return 1/4 * sum;
    }

    std::vector<std::vector<std::vector<double>>> JPEG::getSpatialBlocks(std::vector<Block> frequentialBlocks){

        std::vector<std::vector<std::vector<double>>> spatialBlocks;
        int count = 0;
        for (int i = 0; i < mcuWidth * mcuHeight; i++) {
            for (int j = 0; j < nb_comp; j++) {
                for (int k = 0; k < arrayInfoComposante[colorOrder[j]].fh * arrayInfoComposante[colorOrder[j]].fv; k++) {
                    std::vector<std::vector<double>> colonneX;
                    for (int x = 0; x < 8; x++){
                        std::vector<double> ligneY;
                        for (int y = 0; y < 8; y++){
                            ligneY.push_back(InverseQuantisationCosinusTransform(x, y,
                                                                                 arrayInfoComposante[colorOrder[j]].iq,
                                                                                 frequentialBlocks[count]));
                        }
                        colonneX.push_back(ligneY);
                    }
                    spatialBlocks.push_back(colonneX);
                    count++;
                }
            }
        }
        return spatialBlocks;
    }

    YCbCr JPEG::upscaleByComponent(std::vector<std::vector<std::vector<double>>> spatialBlocks){

    }


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



// jpeg