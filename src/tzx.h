//-*- C++ -*-

#ifndef TZX_H
#define TZX_H

#include <cstdint>
#include <stdio.h>

/*
  Writing to TZX files.
 */
class TzxWriter {

private:

    FILE* file;

    void printBlockInfo(unsigned char* buffer, unsigned int size,
                        unsigned int pause);
    void writeTAP(unsigned char* buffer, unsigned int size);

    void write_byte(std::uint8_t);
    void write_2bytes(std::uint16_t);
    void write_3bytes(std::uint32_t);

public:

    TzxWriter(const char *);
    ~TzxWriter();

    void writeID10(unsigned char* buffer, unsigned int size, unsigned int pause);
};


#endif
