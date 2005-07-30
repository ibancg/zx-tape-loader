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
