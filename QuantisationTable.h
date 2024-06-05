//
// Created by Tom Favereau on 22/05/2024.
//

#ifndef FILE_COMPRESSION_CPP_QUANTISATIONTABLE_H
#define FILE_COMPRESSION_CPP_QUANTISATIONTABLE_H
#include <cstdint>
#include <vector>
namespace quantisation_table {

    class QuantisationTable {
        bool precision;
        uint8_t iq; //indice de la table
        std::vector<char> values;
    public:
        explicit QuantisationTable(std::vector<char> sector);
        static int access(int i, int j);
    };



} // quantisation_table

#endif //FILE_COMPRESSION_CPP_QUANTISATIONTABLE_H
