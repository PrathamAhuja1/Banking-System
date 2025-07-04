add_library(compression
    Huffman.cpp 
    Huffman.h
)
target_include_directories(compression PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
