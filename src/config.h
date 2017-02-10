//-*- C++ -*-

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
