//-*- C++ -*-
/*
  lingot, a musical instrument tuner.

  Copyright (C) 2004, 2005  Ibán Cereijo Graña, Jairo Chapela Martínez.

  This file is part of lingot.

  lingot is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  lingot is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with lingot; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdlib.h>
#include <string.h>

#include "tzx.h"

TzxWriter::TzxWriter(char* file)
{
  if ((f = fopen(file, "wb")) == NULL) {
    char buff[80];
    sprintf(buff, "couldn't open output file %s:", file);
    perror(buff);
    exit(-1);
  }

  const char* header_str = "ZXTape!";
  fwrite(header_str, strlen(header_str), 1, f);
  write_byte(0x1a);
  write_byte(1);  // major
  write_byte(13); // minor
}

TzxWriter::~TzxWriter()
{
  fclose(f);
}

//----------------------------------------------------------------------------

void TzxWriter::printBlockInfo(unsigned char* buffer,
			       unsigned int size, unsigned int pause)
{
  u_int16_t location = *((u_int16_t*) &buffer[14]);
  u_int16_t next_block_size = *((u_int16_t*) &buffer[12]);
  
  printf("\n\n     Name : ");
  for (int i = 2; i < 12; i++) printf("%c", buffer[i]);
  printf("\n     Type : ");
  switch (buffer[1]) {
    
  case 0: 
    printf("Program ");
    if (location > 9999)
      printf("(no autoexecutable)\n");
    else printf("(autoexecutable at line %u)\n", location);
    break;
  case 1: printf("Numerical Array\n"); break;
  case 2: printf("Alfanumerical array\n"); break;
  case 3: 
    if ((next_block_size == 6912) && (location == 16384))
      printf("SCREEN$\n");
    else printf("Machine code bytes (starting in %u)\n", location);
    break;
  }
  printf("     Size : %u bytes\n\n", next_block_size);
}

//----------------------------------------------------------------------------

void TzxWriter::writeID10(unsigned char* buffer, 
			  unsigned int size, unsigned int pause)
{
  // if the block is a header, show some info.
  if (buffer[0] == 0) printBlockInfo(buffer, size, pause);

  write_byte(0x10); // block id
  write_2bytes(pause);
  write_2bytes(size);
  for (unsigned int i = 0; i < size; i++) write_byte(buffer[i]);
}

//----------------------------------------------------------------------------

void TzxWriter::write_byte(u_int8_t b)
{
  fwrite(&b, 1, 1, f);
}

void TzxWriter::write_2bytes(u_int16_t i)
{
  fwrite(&i, 2, 1, f);
}

void TzxWriter::write_3bytes(u_int32_t l)
{
  write_byte(l & 0xff);
  write_byte((l >> 8) & 0xff);
  write_byte((l >> 16) & 0xff);
}

//----------------------------------------------------------------------------

void TzxWriter::writeTAP(unsigned char* buffer, unsigned int size) {
}
