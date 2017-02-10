//-*- C++ -*-

#ifndef __FILE_H__
#define __FILE_H__

class FileReader
{
private:

  FILE* file;

public:
  
  FileReader(char* name, int* rate);
  ~FileReader();
  
  int read(void* buffer, int size);
};

#endif
