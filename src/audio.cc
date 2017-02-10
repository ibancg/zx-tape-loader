//-*- C++ -*-

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <stdlib.h>

#include "defs.h"
#include "audio.h"

audio::audio(char* fdsp, int* rate, int channels, int format, bool write)
{
  if (write) dsp = open(fdsp, O_RDWR);
  else dsp = open(fdsp, O_RDONLY);

  if (dsp < 0) {
    char buff[80];
    sprintf(buff, "Unable to open audio device %s", fdsp);
    perror(buff);
    exit(-1);
  }

  // or from soundcard, setting some parameters.
  //if (ioctl(dsp, SOUND_PCM_READ_CHANNELS, &channels) < 0)
  if (ioctl(dsp, SNDCTL_DSP_CHANNELS, &channels) < 0) {
    perror("Error setting number of channels");
    exit(-1);
  }
  
  /*  if (ioctl(dsp, SOUND_PCM_WRITE_CHANNELS, &channels) < 0) {
      perror("Error setting number of channels");
      exit(-1);
      }*/
  
  // sample size
  //if (ioctl(dsp, SOUND_PCM_SETFMT, &format) < 0)
  if (ioctl(dsp, SNDCTL_DSP_SETFMT, &format) < 0) {
    perror("Error setting bits per sample");
    exit(-1);
  }

  int fragment_size = 1;
  int DMA_buffer_size = 512;
  int param = 0;

  for (param = 0; fragment_size < DMA_buffer_size; param++)
    fragment_size <<= 1;
  
  param |= 0x00ff0000;
  
  if (ioctl(dsp, SNDCTL_DSP_SETFRAGMENT, &param) < 0) {
    perror("Error setting DMA buffer size");
    exit(-1);
  }
  
  // DMA divisor
  /*  if (ioctl(dsp, SNDCTL_DSP_SUBDIVIDE, &dma) < 0) {
      perror("Error setting DMA divisor");
      exit(-1);
      }
      
      // Rate de muestreo / reproduccion
      if (ioctl(dsp, SOUND_PCM_WRITE_RATE, rate) < 0) {
      perror("Error setting write rate");
      exit(-1);
      }*/

  //  if (ioctl(dsp, SOUND_PCM_READ_RATE, rate) < 0)
  if (ioctl(dsp, SNDCTL_DSP_SPEED, rate) < 0) {
    perror("Error setting sample rate");
    exit(-1);
  }
  
  /*
  // non-blocking reads.
  if (fcntl(dsp, F_SETFL, O_NONBLOCK) < 0) {
  perror("Error setting non-blocking reads");
  exit(-1);
  }
  */
}

audio::~audio()
{
  close(dsp);
}

int audio::read(void *buffer, int size)
{
  return ::read(dsp, buffer, size); // read from soundcard
}

int audio::write(void *buffer, int size)
{
  return ::write(dsp, buffer, size); // write to soundcard
}
