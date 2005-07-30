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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "config.h"

// the following tokens will appear in the config file.
char* Config::token[] = {
  "AUDIO_DEV",
  "SAMPLE_RATE",
  "NOISE_THRESHOLD",
  "PILOT_MIN_EDGES",
  "PILOT_TOLERANCE",
  "BIT_TOLERANCE",
  "PLAY_SAMPLES",
  NULL // NULL terminated array
};

const char* Config::format = "sdfdffd"; // print/scan param formats.

//----------------------------------------------------------------------------

Config::Config() {
  reset(); // set default values.

  // internal parameters associated to each token in the config file.
  void* c_param[] = {
    &AUDIO_DEV,
    &SAMPLE_RATE,
    &NOISE_THRESHOLD,
    &PILOT_MIN_EDGES,
    &PILOT_TOLERANCE,
    &BIT_TOLERANCE,
    &PLAY_SAMPLES
  };
  
  memcpy(param, c_param, 7*sizeof(void*));
}

//----------------------------------------------------------------------------

// default values
void Config::reset()
{
  sprintf(AUDIO_DEV,"%s","/dev/dsp");

  SAMPLE_RATE     = 44100; // Hz
  NOISE_THRESHOLD = 37.0; // dB
  PLAY_SAMPLES    = 0;
  PILOT_MIN_EDGES = 200;
  PILOT_TOLERANCE = 0.4;
  BIT_TOLERANCE   = 0.3;
  updateInternalParameters();
}

//----------------------------------------------------------------------------

void Config::updateInternalParameters()
{
  NOISE_THRESHOLD_UN = pow(10.0, NOISE_THRESHOLD/10.0);
}

//----------------------------------------------------------------------------

void Config::parseConfigFile( char* file )
{
  FILE* fp;

# define MAX_LINE_SIZE 100
  
  char s1[MAX_LINE_SIZE];
  
  if ((fp = fopen(file, "r")) == NULL) {
    sprintf(s1, 
	    "error opening config file %s, assuming default values ", file);
    perror(s1);
    return;
  }

  int line = 0;

  do {
    
    line++;

    if (!fgets(s1, MAX_LINE_SIZE, fp)) break;;

    if (s1[0] == '#') continue;

    char* s2;      // contendrá los tokens dentro de la línea.

    s2 = strtok(s1, " \t=\n");

    if (!s2) continue; // blank line.

    int i;
    for (i = 0; Config::token[i]; i++) if (!strcmp(s2, Config::token[i])) {
      break; // found token.
    }

    if (!Config::token[i]) {
      fprintf(stderr, 
	      "warning: parse error at line %i: unknown keyword %s\n", 
	      line, s2);
      continue;
    }

    // cojo el valor del atributo.
    s2 = strtok(NULL, " \t=\n");
    
    if (!s2) {
      fprintf(stderr, "warning: parse error at line %i: value expected\n", 
	      line);
      continue;
    }

    // asign the value to the parameter.
    switch (Config::format[i]) {
    case 's' : 
      sprintf(((char*) param[i]), "%s", s2);
      break;
    case 'd' : sscanf(s2, "%d", (int*) Config::param[i]); break;
    case 'f' : 
      float aux;
      sscanf(s2, "%f", &aux);
      *((FLT*) Config::param[i]) = aux;
      break;
    }
    
  } while (true);
   
  fclose(fp);

  updateInternalParameters();

# undef MAX_LINE_SIZE
}

//----------------------------------------------------------------------------

void Config::saveConfigFile(char* file) {

  FILE* fp;

  if ((fp = fopen(file, "w")) == NULL) {
    char buff[100];
    sprintf(buff, "error saving config file %s ", file);
    perror(buff);
    return;
  }

  fprintf(fp, "# Config file automatically created by loader\n\n");

  for (int i = 0; token[i]; i++) 
    switch (format[i]) {
    case 's': 
      fprintf(fp, "%s = %s\n", token[i], (char*) param[i]); 
      break;
    case 'd': 
      fprintf(fp, "%s = %d\n", token[i], *((int*) param[i]));
      break;
    case 'f': 
      fprintf(fp, "%s = %0.3f\n", token[i], *((FLT*) param[i])); 
      break;
    }

  fclose(fp);
}
