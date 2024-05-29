//
// Created by Tom Favereau on 22/05/2024.
//

#ifndef FILE_COMPRESSION_CPP_QUANTISATIONTABLE_H
#define FILE_COMPRESSION_CPP_QUANTISATIONTABLE_H
#include <cstdint>

namespace quantisation_table {

    class QuantisationTable {
        bool precision;
        uint8_t iq; //indice de la table

        uint8_t values[64];

    };

} // quantisation_table

#endif //FILE_COMPRESSION_CPP_QUANTISATIONTABLE_H
