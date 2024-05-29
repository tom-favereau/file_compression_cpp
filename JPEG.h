//
// Created by Tom Favereau on 22/05/2024.
//

#ifndef FILE_COMPRESSION_CPP_JPEG_H
#define FILE_COMPRESSION_CPP_JPEG_H
#include <cstdint>
#include <vector>
#include <string>


namespace jpeg {

    struct InfoComposante {
        uint8_t ic; // indice de composante horizontal
        uint8_t fh; // facteur d'échantillonage horizontal
        uint8_t fv; // ___ vertical
        uint8_t iq; // indice quantification
    };

    struct InfoBrut{
        uint8_t ic; //indice composante
        uint8_t ihAC; //indice huffman
        uint8_t ihDC; //indice huffman
    };

    class JPEG {

    private:
        std::string file_name;
        //start of frame
        uint8_t precision;
        uint16_t height;
        uint16_t width;
        uint8_t nb_comp; //couleur ou non
        std::vector<InfoComposante> arrayInfoComposante; //tableau de taille nb_com

        //start of scan
        std::vector<InfoBrut> arrayInfoBrut; //tableau de taille n;


    public:
        JPEG(std::string file_name);

    };

} // jpeg

#endif //FILE_COMPRESSION_CPP_JPEG_H
