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

#ifndef __CORE_H__
#define __CORE_H__

#include <stdio.h>

#include "defs.h"

class Config;
class audio;
class Filter;
class TzxWriter;
class FileReader;

typedef enum { s_noise, s_signal, s_pilot, s_sync1, s_data, s_decode } status_t;

/*
  This object analyzes the input data.
 */
class Core {

private:

  Config*     conf;
  audio*      audio_handler;
  TzxWriter*  tzx_writer; 
  FileReader* file_reader; 

  Filter*  filter;

  status_t status;
  int      sample_index;

  static bool        stop;

  FLT                edges[3]; // last edges stored.

  FLT                pilot_freq;
  FLT                sync1_freq;
  FLT                sync2_freq;
  FLT                bit_freq[2];
  FLT                after_block_pause; // signal parameters.

  unsigned int       pilot_pulses;
  unsigned long int  global_bit_counter;


  int  getSample(FLT* sample, FLT* edge);

  static void ControlC_Callback(int);

public:

  Core(class Config* conf, char* output_file, char* input_file);
  ~Core();

  void start();

};


#endif
