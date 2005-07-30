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

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "file.h"

FileReader::FileReader(char* name, int* rate)
{
  if ((file = fopen(name, "rb")) == NULL) {
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
  fclose(file);
}

int FileReader::read(void *buffer, int size)
{
  return fread(buffer, size, 1, file);
}
