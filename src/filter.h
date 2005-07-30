//-*- C++ -*-
/*
  lingot, a musical instrument tuner.

  Copyright (C) 2004, 2005  Ib�n Cereijo Gra�a, Jairo Chapela Mart�nez.

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

#ifndef __FILTER_H__
#define __FILTER_H__

#include <fcntl.h>
#include <stdlib.h>

#include "defs.h"

/*
  digital filtering implementation.
 */

class Filter {

 private:

 public:

  FLT*  a;
  FLT*  b;  // coefs
  FLT*  s;  // status

  unsigned int N;

  void filter(unsigned int n, FLT* in, FLT* out); // vector filtering
  FLT  filter(FLT in);                            // sample filtering

  Filter();
  Filter(unsigned int Na, unsigned int Nb, FLT* a, FLT* b, FLT* s = NULL);
  void update(unsigned int Na, unsigned int Nb, FLT* a, FLT* b);
  ~Filter();
};

#endif