//-*- C++ -*-

#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <stdio.h>

// class for audio input handling.
class audio
{
private:

  int   dsp;  // file handler.

public:
  
  // open audio dev
  audio(char* fdsp, int* rate, int channels, int format, bool write);
  ~audio();
  
  int read(void* buffer, int size);
  int write(void* buffer, int size);
};

#endif
