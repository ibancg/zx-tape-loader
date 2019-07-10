//-*- C++ -*-

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "file.h"

FileReader::FileReader(const char* name, int* rate)
{
    file = fopen(name, "rb");
    if (!file) {
        char buff[80];
        sprintf(buff, "couldn't open input file %s:", name);
        perror(buff);
        exit(-1);
    }

    unsigned short int wav_header[29];
    fread(wav_header, 2, 29, file);
    *rate = wav_header[12];
    printf("WAV sample rate: %i\n", *rate);
}

FileReader::~FileReader()
{
    if (file) {
        fclose(file);
    }
}

int FileReader::read(void *buffer, int size)
{
    return fread(buffer, size, 1, file);
}
