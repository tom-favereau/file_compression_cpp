//
// Created by Tom Favereau on 22/05/2024.
//

#include "QuantisationTable.h"
#include "ByteReading.h"
namespace quantisation_table {
    QuantisationTable::QuantisationTable(std::vector<char> sector){
        precision = ByteReading::readByte(sector[4], 0, 4) == 0;
        iq = ByteReading::readByte(sector[4], 4, 4);
        values = {sector.begin() + 5, sector.end()};
    }

    int QuantisationTable::access(const int i, const int j) {
        if (i == 0 && j == 0) {
            return 0;
        } else {
            int N = 0;
            for (int k = 1; k < i + j; k++){
                if (k < 8) {
                    N += k + 1;
                } else {
                    N += ((- k) % 7 + 7) % 7 + 1;
                }
            }
            if (i + j < 8) {
                if ((i + j) % 2) {
                    return N + i + 1;
                } else {
                    return N + j + 1;
                }
            } else {
                if ((i + j) % 2) {
                    return N + 8 - j;
                } else {
                    return N + 8 - i;
                }
            }
        }
    }
} // quantisation_table