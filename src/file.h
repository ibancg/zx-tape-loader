//-*- C++ -*-

#ifndef FILE_H
#define FILE_H

#include <stdio.h>

class FileReader
{
private:

    FILE* file;

public:

    FileReader(const char *name, int* rate);
    ~FileReader();

    int read(void* buffer, int size);
};

#endif
