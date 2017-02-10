//-*- C++ -*-

#ifndef __TZX_H__
#define __TZX_H__

#include <stdio.h>

/*
  Writing to TZX files.
 */
class TzxWriter {

private:

  FILE* f;

  void printBlockInfo(unsigned char* buffer, unsigned int size,
		      unsigned int pause);
  void writeTAP(unsigned char* buffer, unsigned int size);

public:

  TzxWriter(char*);
  ~TzxWriter();

  void writeID10(unsigned char* buffer, unsigned int size, unsigned int pause);

  void write_byte(u_int8_t);
  void write_2bytes(u_int16_t);
  void write_3bytes(u_int32_t);
};


#endif
