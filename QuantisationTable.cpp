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
} // quantisation_table