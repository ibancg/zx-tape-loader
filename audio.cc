
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/soundcard.h>

#include "audio.h"

audio::audio(int canales, int frecuencia, int formato)
{
  int dma  = 4;

  audio::freq = frecuencia;

  // Abre el dsp
  dsp = open("/dev/dsp", O_RDWR);
  if (dsp < 0)
  {
    perror("No puedo abrir /dev/dsp");
    exit(-1);
  }

  // Los canales...
  if (ioctl(dsp, SOUND_PCM_READ_CHANNELS, &canales) < 0)
  {
    perror("error al poner el número de canales");
    exit(-1);
  }

  if (ioctl(dsp, SOUND_PCM_WRITE_CHANNELS, &canales) < 0)
  {
    perror("error al poner el número de canales");
    exit(-1);
  }

  // Tamaño de las muestras
  if (ioctl(dsp, SOUND_PCM_SETFMT, &formato) < 0)
  {
    perror("error al poner los bits por muestra");
    exit(-1);
  }

  // Divisor de DMA
  if (ioctl(dsp, SOUND_PCM_SUBDIVIDE, &dma) < 0)
  {
    perror("error al poner el divisor de dma");
    exit(-1);
  }

  // Frecuencia de muestreo / reproduccion
  if (ioctl(dsp, SOUND_PCM_WRITE_RATE, &freq) < 0)
  {
    perror("error al poner la frecuencia de reproducción");
    exit(-1);
  }

  if (ioctl(dsp, SOUND_PCM_READ_RATE, &freq) < 0)
  {
    perror("error al poner la frecuencia de muestreo");
    exit(-1);
  }
}

audio::~audio()
{
 close(dsp);
}

void audio::lee(void *buffer, int tamanho)
{
 read(dsp, buffer, tamanho);
}

void audio::escribe(void *buffer, int tamanho)
{
 write(dsp, buffer, tamanho);
}
