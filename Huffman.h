//
// Created by Tom Favereau on 22/05/2024.
//

#ifndef FILE_COMPRESSION_CPP_HUFFMAN_H
#define FILE_COMPRESSION_CPP_HUFFMAN_H
#include <cstdint>
#include <vector>
#include <unordered_map>



    typedef struct _huff_table_t {
        uint16_t code;
        uint8_t value;
        uint8_t is_elt;
        struct _huff_table_t *parent;
        struct _huff_table_t *left;
        struct _huff_table_t *right;
    } huff_table_t;




    class Huffman {
    private:
        //sur un seul octet on a ces information
        uint8_t indice_huffman{};
        bool type{}; // 0 = DC, 1 = AC

        uint8_t nb_symboles[16]{};

        std::unordered_map<uint16_t , uint8_t> huffman_codes;

        void buildCode(const std::vector<char>& sector);



    public:
        Huffman(const std::vector<char>& sector);

        uint8_t find(const uint16_t code) const;

        bool contains(const uint16_t code) const;

        bool isAC();

    };


#endif //FILE_COMPRESSION_CPP_HUFFMAN_H
