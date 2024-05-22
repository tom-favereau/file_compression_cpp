cmake_minimum_required(VERSION 3.25)
project(file_compression_cpp)

set(CMAKE_CXX_STANDARD 23)

add_executable(file_compression_cpp main.cpp Huffman.cpp Huffman.h MCU.cpp MCU.h JPEG.cpp JPEG.h TIFF.cpp TIFF.h QuantisationTable.cpp QuantisationTable.h)
