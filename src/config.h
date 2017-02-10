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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "defs.h"

/*
  Configuration class. Determines the loader's behaviour.
  Some parameters are internal only.
 */
class Config {

private:

  static const char* token[];   // config file tokens array.
  void*        param[7]; // parameter pointer array.

  // formats to scan/print tokens from/to the config file.
  static const char* format;

public:
  
  char         AUDIO_DEV[80]; // default "/dev/dsp"
  
  int          SAMPLE_RATE;   // soundcard sample rate.

  FLT          NOISE_THRESHOLD;    // dB
  FLT          NOISE_THRESHOLD_UN; // natural units (internal parameter)

  int          PILOT_MIN_EDGES; // minimum edges of a PILOT pulse.
  FLT          PILOT_TOLERANCE; // pulse period tolerance to detect end of PILOT.
  FLT          BIT_TOLERANCE;   // pulse period tolerance to detect bit changes.

  int          PLAY_SAMPLES;

public:
  
  Config();

  // back to default configuration.
  void reset();

  // derivate internal parameters from external ones.
  void updateInternalParameters();

  void saveConfigFile(char* archivo);
  void parseConfigFile(char* archivo);
};


#endif
