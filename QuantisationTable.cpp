//
// Created by Tom Favereau on 22/05/2024.
//

#include "QuantisationTable.h"
#include "ByteReading.h"

/**
 * Constructs a QuantisationTable instance from a DQT sector.
 * @param sector DQT sector as a vector of char.
 */
    QuantisationTable::QuantisationTable(std::vector<char> sector){
        precision = ByteReading::readByte(sector[4], 0, 4) == 0;
        iq = ByteReading::readByte(sector[4], 4, 4);
        values = {sector.begin() + 5, sector.end()};
    }

    /**
     * Access to the table's value from matrix coordinates.
     * @param i row index, int.
     * @param j column index, int.
     * @return the index in the zigzag representation of a quantisation table of value at (i, j)
     * in the matrix representation of the same quantisation table.
     */
    int QuantisationTable::access(const int i, const int j) {
        //i + j gives the diagonal (bottom left to upper right) to consider
        //Odd diagonals are different from even diagonals because the zigzag order is inverted.
        if (i == 0 && j == 0) {
            return 0;
        } else {
            //Here, the number of previous values before the current diagonal are counted.
            int N = 0;
            for (int k = 1; k < i + j; k++){
                if (k < 8) {
                    N += k + 1;
                } else {
                    N += ((- k) % 7 + 7) % 7 + 1;
                }
            }
            //Then, remaining values to value at (i, j) are counted.
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


    /**
     * Returns the value at (i, j) in the matrix representation of a quantisation table
     * @param i row index
     * @param j column index
     * @return value at (i, j)
     */
    uint8_t QuantisationTable::elementAt(int i, int j) const{
        return values[access(i, j)];
    }
