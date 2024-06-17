//
// Created by Tom Favereau on 22/05/2024.
//

#ifndef FILE_COMPRESSION_CPP_HUFFMAN_H
#define FILE_COMPRESSION_CPP_HUFFMAN_H
#include <cstdint>
#include <vector>
#include <unordered_map>


    //UNUSED
    typedef struct _huff_table_t {
        uint16_t code;
        uint8_t value;
        uint8_t is_elt;
        struct _huff_table_t *parent;
        struct _huff_table_t *left;
        struct _huff_table_t *right;
    } huff_table_t;



/**
 * Class for a Huffman table defined in a DHT section.
 */
    class Huffman {
    private:
        //sur un seul octet on a ces information
        //Index of the table
        uint8_t indice_huffman{};
        //Type of Huffman table
        bool type{}; // 0 = DC, 1 = AC

        //Number of symbols of a given size ranging from 1 to 16.
        uint8_t nb_symboles[16]{};

        //Hashmap where the keys are codes and values are encoded values.
        std::unordered_map<uint16_t, uint8_t> huffman_codes;

        void buildCode(const std::vector<char>& sector);

        //Storing codes in a vector of vector: huffman_offset[i] contains the vector of all symbols of length i + 1.
        //huffman_offset is needed because, for example 0 and 000 are two different symbols of different length.
        //As codes are stored in uint16_t, we thus need to discriminate valid codes from not fully read codes by the
        //length of the symbol.
        std::vector<std::vector<uint16_t>> huffman_offset;



    public:
        Huffman(const std::vector<char>& sector);

        uint8_t find(const uint16_t code) const;

        bool contains(uint16_t code, int size) const;

        bool isAC();

    };


#endif //FILE_COMPRESSION_CPP_HUFFMAN_H
